#ifndef GME_H
#define GME_H

#include "brd.h"

typedef struct gme_state_s {
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
  state.status = brd_apply(&mve, state.board);
  state.side *= -1;
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
    if (delta < 1) return;

    // FIXME: Enemy purges piece on second turn
    gme_do(gme_tick_enemy.from, gme_tick_enemy.to);
    gme_tick_enemy.timestamp = 0;
    return;
  }

  unsigned brd[8 * 8];
  int from = -1, to = -1;
  int mx = -100000;
  for (int i = 0; i < 8 * 8; i++) {
    unsigned b = state.board[i];
    if (MVE_DIR(b) != state.side) continue;
    for (int j = 0; j < 8 * 8; j++) {
      if (!brd_can_move(state.board, i, j)) continue;

      mve_t mve; mve_new(&mve, state.board, i, j);
      unsigned p, n;
      brd_apply(&mve, brd);
      brd_score(brd, &p, &n);
      int score = (int)p - (int)n;
      // TODO: if eq and random?
      if (score > mx) {
        mx = score;
        from = i;
        to = j;
      }
    }
  }
  gme_tick_enemy.timestamp = tim_now();
  gme_tick_enemy.from = from;
  gme_tick_enemy.to   = to;
  printf("%d %d -- %d\n", from, to, mx);
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
  if (state.status == brd_s_checkmate) return;
  // TODO if (state.side == 1) return;

  int hover = gme_board_pos(px, py);
  if (hover == -1) return;
  int b = state.board[hover];

  if (state.pick == -1) {
    if (!b || (MVE_DIR(b) != state.side)) return;
    state.hover = hover;
    return;
  }

  if (!brd_can_move(state.board, state.pick, hover)) return;

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

  gme_do(state.pick, state.hover);
  state.pick = state.hover = -1;
}

void gme_mouse_cancel(void) {
  state.pick = state.hover = -1;
}

const gme_state_t * gme_state() { return &state; }

#endif
#endif
