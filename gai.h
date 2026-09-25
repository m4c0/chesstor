#ifndef GAI_H
#define GAI_H

#define GAI_REV "ai v3"

typedef struct gai_s {
  unsigned from;
  unsigned to;
} gai_t;

int gai_init(void);
int gai_reset(void);
int gai_tick(const unsigned * board, int side, gai_t * res);

#ifdef GAI_IMPL

#include "brd.h"
#include "mve.h"
#include "opn.h"

int gai_init(void) {
  return 0;
}

int gai_reset(void) {
  return 0;
}

int gai_tick(const unsigned * board, int side, gai_t * res) {
  const opn_t * opn = opn_pick(board);
  if (opn) {
    if (MVE_DIR(board[opn->from]) == side) { // panic check
      res->from = opn->from;
      res->to   = opn->to;
      return 1;
    }
  }

  unsigned brd[8 * 8];

  int score_rnd = 1;
  int score_from = -1, score_to = -1;
  int score_mx = -100000;

  for (int i = 0; i < 8 * 8; i++) {
    unsigned b = board[i];
    if (MVE_DIR(b) != side) continue;
    for (int j = 0; j < 8 * 8; j++) {
      if (!brd_can_move(board, i, j)) continue;

      mve_t mve; mve_new(&mve, board, i, j);
      brd_apply(&mve, brd);

      brd_status_t s = brd_status(brd, side);
      if (s != brd_s_normal) continue;

      // TODO: should we take stalemate as a valid move condition?
      if (brd_status(brd, -side) == brd_s_checkmate) {
        res->from = i;
        res->to   = j;
        return 1;
      }

      unsigned capture = board[j];
      unsigned valid = 1;
      for (int k = 0; k < 8 * 8; k++) {
        if (!brd_can_move(brd, k, j)) continue;
        valid = (capture & 0xF) > (b & 0xF);
      }
      if (!valid) continue;

      unsigned p, n;
      brd_score(brd, &p, &n);
      int score = (int)p - (int)n;
      if (score > score_mx || (score == score_mx && (rand() % score_rnd))) {
        score_rnd = (score == score_mx) ? 1 : score_rnd + 1;
        score_mx = score;
        score_from = i;
        score_to = j;
      }
    }
  }

  res->from = score_from;
  res->to   = score_to;
  return 1;
}

#endif
#endif
