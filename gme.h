#ifndef GME_H
#define GME_H

#include "brd.h"

typedef struct gme_state_s {
  float mouse_x, mouse_y;
  unsigned board[8 * 8];
  unsigned hover;
  unsigned pick;
  int side;
  brd_status_t status;
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
#include "gai.h"
#include "mve.h"
#include "tim.h"

gme_state_t state;

void gme_reset(void) {
  brd_reset(state.board);

  state.hover = -1;
  state.pick = -1;
  state.side = -1;
  state.status = brd_s_normal;

  tim_now(); // inits
}

static void gme_do(unsigned from, unsigned to) {
  mve_t mve; mve_new(&mve, state.board, from, to);
  brd_apply(&mve, state.board);
  state.status = brd_status(state.board, -state.side);
  state.side *= -1;
  state.pick = state.hover = -1;
}

static float gme_board_norm(float p) {
  p /= 0.9 - 0.07;
  p = p * 0.5 + 0.5;
  p *= 8;
  return p;
}
static float gme_board_unorm(float p) {
  p /= 8;
  p = p * 2 - 1;
  p *= 0.9 - 0.07;
  return p;
}

struct {
  float timestamp;
  int from, to;
} gme_tick_enemy = {0};
void gme_tick(void) {
  if (state.side == -1) return;
  if (state.status == brd_s_checkmate) return;

  if (gme_tick_enemy.timestamp > 0) {
    float delta = (tim_now() - gme_tick_enemy.timestamp) / 0.3f;
    if (delta < 1) {
      float ix = 0.5 + (gme_tick_enemy.to % 8) * delta + (gme_tick_enemy.from % 8) * (1 - delta);
      float iy = 0.5 + (gme_tick_enemy.to / 8) * delta + (gme_tick_enemy.from / 8) * (1 - delta);
      state.pick  = gme_tick_enemy.from;
      state.hover = gme_tick_enemy.to;
      state.mouse_x = gme_board_unorm(ix);
      state.mouse_y = gme_board_unorm(iy);
      return;
    }

    gme_do(gme_tick_enemy.from, gme_tick_enemy.to);
    gme_tick_enemy.timestamp = 0;
    gme_tick_enemy.from = gme_tick_enemy.to = -1;
    return;
  }

  gai_t gai = {0};
  if (gai_tick(state.board, state.side, &gai)) {
    gme_tick_enemy.timestamp = tim_now();
    gme_tick_enemy.from      = gai.from;
    gme_tick_enemy.to        = gai.to;
  }
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
  if (state.side == 1) return; 

  state.hover = -1;
  if (state.status == brd_s_checkmate) return;
  // TODO if (state.side == 1) return;

  int hover = gme_board_pos(px, py);
  if (hover == -1) return;
  int b = state.board[hover];

  state.mouse_x = px;
  state.mouse_y = py;

  if (state.pick == -1) {
    if (!b || (MVE_DIR(b) != state.side)) return;
    state.hover = hover;
    return;
  }

  if (!brd_can_move(state.board, state.pick, hover)) return;

  state.hover = hover;
}

void gme_mouse_down() {
  if (state.side == 1) return; 

  if (state.hover == -1) {
    state.pick = state.hover = -1;
    return;
  }
  state.pick = state.hover;
  state.hover = -1;
}

void gme_mouse_up(void) {
  if (state.side == 1) return; 

  if (state.pick == -1 || state.hover == -1) {
    state.pick = state.hover = -1;
    return;
  }

  gme_do(state.pick, state.hover);
}

void gme_mouse_cancel(void) {
  if (state.side == 1) return; 

  state.pick = state.hover = -1;
}

const gme_state_t * gme_state() { return &state; }

#endif
#endif
