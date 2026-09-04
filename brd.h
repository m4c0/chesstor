#ifndef BRD_H
#define BRD_H

typedef struct mve_s mve_t;

void brd_reset(unsigned * brd);
void brd_apply(const mve_t * mve);
int brd_in_check(unsigned * brd, int dir);

#ifdef BRD_IMPL
#include "mve.h"

static unsigned template[8 * 2] = {
  mve_p_rook, mve_p_knit, mve_p_bish, mve_p_quen, mve_p_king, mve_p_bish, mve_p_knit, mve_p_rook,
  mve_p_pawn, mve_p_pawn, mve_p_pawn, mve_p_pawn, mve_p_pawn, mve_p_pawn, mve_p_pawn, mve_p_pawn,
};
void brd_reset(unsigned * brd) {
  for (int i = 0; i < 8 * 8; i++) brd[i] = 0;

  for (int i = 0; i < 8; i++) {
    brd[i     ] = template[i    ];
    brd[i +  8] = template[i + 8];
    brd[i + 48] = template[i + 8] | 0x80;
    brd[i + 56] = template[i    ] | 0x80;
  }
}

static inline int pawn_conversion(const mve_t * mve) {
  if ((mve->piece & 0xF) != mve_p_pawn) return 0;
  if (mve->to_y == 0 && mve->dir == -1) return 1;
  if (mve->to_y == 7 && mve->dir ==  1) return 1;
  return 0;
}
static inline void castling(const mve_t * mve) {
  if ((mve->piece & 0xF) != mve_p_king) return;
  if (mve->dx == -2) {
    mve->board[mve->from_y * 8 + 3] = mve->board[mve->from_y * 8] | 0x40;
    mve->board[mve->from_y * 8] = 0;
  }
  if (mve->dx == 2) {
    mve->board[mve->from_y * 8 + 5] = mve->board[mve->from_y * 8 + 7] | 0x40;
    mve->board[mve->from_y * 8 + 7] = 0;
  }
}
void brd_apply(const mve_t * mve) {
  castling(mve);

  unsigned piece = mve->piece;
  if (pawn_conversion(mve)) piece = ((mve->piece & 0xF0) | mve_p_quen);

  mve->board[mve->to] = piece | 0x40;
  mve->board[mve->from] = 0;
}

int brd_in_check(unsigned * brd, int dir) {
  int king;
  for (king = 0; king < 8 * 8; king++) {
    unsigned b = brd[king];
    if (!b) continue;
    if (dir != MVE_DIR(b)) continue;
    if ((b & 0xF) == mve_p_king) break;
  }
  if (king == 8 * 8) return 1; // Should never happen

  for (int i = 0; i < 8 * 8; i++) {
    unsigned b = brd[i];
    if (!b) continue;
    if (dir == MVE_DIR(b)) continue;

    mve_t mve; mve_new(&mve, brd, i, king);
    if (mve_is_valid(&mve)) return 1;
  }
  return 0;
}

#endif
#endif
