#ifndef GAI_H
#define GAI_H

#define GAI_REV "ai v2"

typedef struct gai_s {
  unsigned from;
  unsigned to;
} gai_t;

int gai_tick(const unsigned * board, int side, gai_t * res);

#ifdef GAI_IMPL

#include "brd.h"
#include "mve.h"

int gai_tick(const unsigned * board, int side, gai_t * res) {
  unsigned brd[8 * 8];
  int from = -1, to = -1;
  int mx = -100000;

  for (int i = 0; i < 8 * 8; i++) {
    unsigned b = board[i];
    if (MVE_DIR(b) != side) continue;
    for (int j = 0; j < 8 * 8; j++) {
      if (!brd_can_move(board, i, j)) continue;

      mve_t mve; mve_new(&mve, board, i, j);
      brd_apply(&mve, brd);

      brd_status_t s = brd_status(brd, side);

      unsigned p, n;
      brd_score(brd, &p, &n);
      if (s != brd_s_normal) continue;
      int score = (int)p - (int)n;
      // TODO: if eq and random?
      if (score > mx) {
        mx = score;
        from = i;
        to = j;
      }
    }
  }

  res->from = from;
  res->to = to;
  return 1;
}

#endif
#endif
