#ifndef BRD_H
#define BRD_H

typedef enum brd_status_e {
  brd_s_normal,
  brd_s_check,
  brd_s_checkmate,
} brd_status_t;

typedef struct mve_s mve_t;

void brd_reset(unsigned * brd);
brd_status_t brd_apply(const mve_t * mve, unsigned * into);
int brd_in_check(const unsigned * brd, int dir);
int brd_in_checkmate(const unsigned * brd, int dir);
int brd_moves_to_check(const unsigned * brd, int from, int to);

void brd_score(const unsigned * brd, unsigned * pos, unsigned * neg);

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

  // Check in one
  // brd[13] = mve_p_quen;
  // brd[21] = mve_p_quen;
  // brd[53] = 0;
  // brd[62] = brd[61] = 0;
}

static inline int pawn_conversion(const mve_t * mve) {
  if (!MVE_PEQ(mve->piece, mve_p_pawn)) return 0;
  if (mve->to_y == 0 && mve->dir == -1) return 1;
  if (mve->to_y == 7 && mve->dir ==  1) return 1;
  return 0;
}
static inline void castling(const mve_t * mve, unsigned * into) {
  if (!MVE_PEQ(mve->piece, mve_p_king)) return;
  if (mve->dx == -2) {
    into[mve->from_y * 8 + 3] = mve->board[mve->from_y * 8] | 0x40;
    into[mve->from_y * 8] = 0;
  }
  if (mve->dx == 2) {
    into[mve->from_y * 8 + 5] = mve->board[mve->from_y * 8 + 7] | 0x40;
    into[mve->from_y * 8 + 7] = 0;
  }
}
brd_status_t brd_apply(const mve_t * mve, unsigned * into) {
  if (into != mve->board) memcpy(into, mve->board, 8 * 8 * 4);
  castling(mve, into);

  unsigned piece = mve->piece;
  if (pawn_conversion(mve)) piece = ((mve->piece & 0xF0) | mve_p_quen);

  into[mve->to] = piece | 0x40;
  into[mve->from] = 0;

  int in_check = brd_in_check(mve->board, -mve->dir);
  if (!in_check) return brd_s_normal;

  return brd_in_checkmate(mve->board, -mve->dir) ? brd_s_checkmate : brd_s_check; 
}

int brd_in_check(const unsigned * brd, int dir) {
  int king;
  for (king = 0; king < 8 * 8; king++) {
    unsigned b = brd[king];
    if (!b) continue;
    if (dir != MVE_DIR(b)) continue;
    if (MVE_PEQ(b, mve_p_king)) break;
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

int brd_in_checkmate(const unsigned * brd, int dir) {
  for (int i = 0; i < 8 * 8; i++) {
    if (MVE_DIR(brd[i]) != dir) continue;

    // TODO: optimise based on piece type
    for (int j = 0; j < 8 * 8; j++) {
      if (!brd_moves_to_check(brd, i, j)) return 0;
    }
  }
  return 1;
}

int brd_moves_to_check(const unsigned * brd, int from, int to) {
  mve_t mve; mve_new(&mve, brd, from, to);
  if (!mve_is_valid(&mve)) return 0;

  unsigned brd2[8 * 8];

  if (brd_apply(&mve, brd2) != brd_s_normal) return 1;

  if (MVE_PEQ(brd[from], mve_p_king) && abs(mve.dx) == 2) {
    mve.dx /= 2;
    mve_new(&mve, brd, from, to - mve.dx);
    return brd_apply(&mve, brd2) != brd_s_normal;
  }

  return 0;
}

void brd_score(const unsigned * brd, unsigned * pos, unsigned * neg) {
  *pos = 0;
  *neg = 0;

  for (int i = 0; i < 8 * 8; i++) {
    unsigned b = brd[i];
    if (!b) continue;

    unsigned * s = MVE_DIR(b) == -1 ? neg : pos;

    for (int j = 0; j < 8 * 8; j++) {
      mve_t mve; mve_new(&mve, brd, i, j);
      if (!mve_is_valid(&mve)) continue;
      if (brd_moves_to_check(brd, i, j)) continue;
      (*s)++;
    }
  }
}

#endif
#endif
