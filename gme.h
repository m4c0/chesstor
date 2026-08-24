#ifndef GME_H
#define GME_H

typedef struct gme_board_s {
  unsigned data[8 * 8];
} gme_board_t;

typedef struct gme_state_s {
  unsigned hover;
  unsigned pick;
} gme_state_t;

const gme_state_t * gme_state();
const gme_board_t * gme_board();

void gme_reset(void);
void gme_tick(void);

void gme_mouse_move(float px, float py);
void gme_mouse_cancel(void);
void gme_mouse_down(void);
void gme_mouse_up(void);

#ifdef GME_IMPL
#include "mve.h"

gme_board_t board;
gme_state_t state;

static unsigned template[8 * 2] = {
  mve_p_towr, mve_p_knit, mve_p_bish, mve_p_quen, mve_p_king, mve_p_bish, mve_p_knit, mve_p_towr,
  mve_p_pawn, mve_p_pawn, mve_p_pawn, mve_p_pawn, mve_p_pawn, mve_p_pawn, mve_p_pawn, mve_p_pawn,
};
void gme_reset(void) {
  for (int i = 0; i < 8 * 8; i++) board.data[i] = 0;

  for (int i = 0; i < 8; i++) {
    board.data[i     ] = template[i    ];
    board.data[i +  8] = template[i + 8];
    board.data[i + 48] = template[i + 8] | 0x80;
    board.data[i + 56] = template[i    ] | 0x80;
  }

  state.hover = -1;
  state.pick = -1;
}

void gme_tick(void) {
}

static int brd_norm(float p) {
  p /= 0.9 - 0.07;
  p = p * 0.5 + 0.5;
  p *= 8;
  return (int)p;
}
static int brd_pos(float px, float py) {
  int bx = brd_norm(px);
  int by = brd_norm(py);
  if (bx < 0) return -1;
  if (by < 0) return -1;
  if (bx > 7) return -1;
  if (by > 7) return -1;
  return by * 8 + bx;
}
void gme_mouse_move(float px, float py) {
  state.hover = -1;

  int hover = brd_pos(px, py);
  if (hover == -1) return;
  int b = board.data[hover];

  if (state.pick == -1) {
    //if (!b || !(b & 0x80)) return;
    if (!b) return; // Enable moving all pieces until we actually game
    state.hover = hover;
    return;
  }

  mve_t mve; mve_new(&mve, board.data, state.pick, hover);
  if (mve_is_valid(&mve)) {
    state.hover = hover;
    return;
  }
}

static inline int gme_pawn_conversion(const mve_t * mve) {
  if ((mve->piece & 0xF) != mve_p_pawn) return 0;
  if (mve->to_y == 0 && mve->dir == -1) return 1;
  if (mve->to_y == 7 && mve->dir ==  1) return 1;
  return 0;
}
static inline void gme_castling(const mve_t * mve) {
  if ((mve->piece & 0xF) != mve_p_king) return;
  if (mve->dx == -2) {
    board.data[mve->from_y * 8 + 3] = board.data[mve->from_y * 8] | 0x40;
    board.data[mve->from_y * 8] = 0;
  }
  if (mve->dx == 2) {
    board.data[mve->from_y * 8 + 5] = board.data[mve->from_y * 8 + 7] | 0x40;
    board.data[mve->from_y * 8 + 7] = 0;
  }
}
void gme_mouse_down() {
  if (state.hover == -1) {
    state.pick = state.hover = -1;
    return;
  }
  state.pick = state.hover;
  state.hover = -1;
  return;

}

void gme_mouse_up(void) {
  if (state.pick == -1 || state.hover == -1) {
    state.pick = state.hover = -1;
    return;
  }

  mve_t mve; mve_new(&mve, board.data, state.pick, state.hover);

  if (gme_pawn_conversion(&mve)) mve.piece = ((mve.piece & 0xF0) | mve_p_quen);
  gme_castling(&mve);

  board.data[state.hover] = mve.piece | 0x40;
  board.data[state.pick] = 0;
  state.pick = state.hover = -1;
}

void gme_mouse_cancel(void) {
  state.pick = state.hover = -1;
}

const gme_board_t * gme_board() { return &board; }

const gme_state_t * gme_state() { return &state; }

#endif
#endif
