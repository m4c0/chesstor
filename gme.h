#ifndef GME_H
#define GME_H

typedef enum gme_status_e {
  gme_s_normal,
  gme_s_check,
  gme_s_checkmate,
} gme_status_t;

typedef struct gme_state_s {
  unsigned board[8 * 8];
  unsigned hover;
  unsigned pick;
  int side;
  gme_status_t status;
} gme_state_t;

const gme_state_t * gme_state();

void gme_reset(void);
void gme_tick(void);

void gme_mouse_move(float px, float py);
void gme_mouse_cancel(void);
void gme_mouse_down(void);
void gme_mouse_up(void);

#ifdef GME_IMPL
#include "brd.h"
#include "mve.h"

gme_state_t state;

void gme_reset(void) {
  brd_reset(state.board);

  state.hover = -1;
  state.pick = -1;
  state.side = -1;
  state.status = gme_s_normal;
}

void gme_tick(void) {
}

static float gme_board_norm(float p) {
  p /= 0.9 - 0.07;
  p = p * 0.5 + 0.5;
  p *= 8;
  return p;
}
static int gme_board_pos(float px, float py) {
  float bx = gme_board_norm(px);
  float by = gme_board_norm(py);
  if (bx < 0) return -1;
  if (by < 0) return -1;
  if (bx >= 8) return -1;
  if (by >= 8) return -1;
  return (int)by * 8 + (int)bx;
}
void gme_mouse_move(float px, float py) {
  state.hover = -1;

  int hover = gme_board_pos(px, py);
  if (hover == -1) return;
  int b = state.board[hover];

  // TODO: only king if check
  if (state.pick == -1) {
    if (!b || (MVE_DIR(b) != state.side)) return;
    state.hover = hover;
    return;
  }

  mve_t mve; mve_new(&mve, state.board, state.pick, hover);
  if (!mve_is_valid(&mve)) return;

  unsigned brd2[8 * 8];
  memcpy(brd2, state.board, 8 * 8 * 4);
  mve.board = brd2;
  brd_apply(&mve);
  if (brd_in_check(brd2, state.side)) return;

  state.hover = hover;
}

void gme_mouse_down() {
  if (state.hover == -1) {
    state.pick = state.hover = -1;
    return;
  }
  state.pick = state.hover;
  state.hover = -1;
}

void gme_mouse_up(void) {
  if (state.pick == -1 || state.hover == -1) {
    state.pick = state.hover = -1;
    return;
  }

  mve_t mve; mve_new(&mve, state.board, state.pick, state.hover);
  brd_apply(&mve);

  state.pick = state.hover = -1;
  state.side *= -1;
  state.status = gme_s_normal; // TODO
}

void gme_mouse_cancel(void) {
  state.pick = state.hover = -1;
}

const gme_state_t * gme_state() { return &state; }

#endif
#endif
