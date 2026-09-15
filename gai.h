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
#include "opn.h"

typedef struct gai_chk_s {
} gai_chk_t;
void gai_chk(const unsigned * board, unsigned p, gai_chk_t * chk) {
  for (int i = 0; i < 8 * 8; i++) {
    // TODO: check if under attack
    // TODO: check if attacks more valueable pieces
    // TODO: check if attackes more than one piece
  }
}

int gai_tick(const unsigned * board, int side, gai_t * res) {
  const opn_mve_t * opn = opn_pick(board);
  if (opn) {
    if (MVE_DIR(board[opn->from]) == side) { // panic check
      res->from = opn->from;
      res->to   = opn->to;
      return 1;
    }
  }

  unsigned brd[8 * 8];
  int score_from = -1, score_to = -1;
  int score_mx = -100000;

  for (int i = 0; i < 8 * 8; i++) {
    unsigned b = board[i];
    if (MVE_DIR(b) != side) continue;
    for (int j = 0; j < 8 * 8; j++) {
      if (!brd_can_move(board, i, j)) continue;

      // TODO: check if we are going to capture

      mve_t mve; mve_new(&mve, board, i, j);
      brd_apply(&mve, brd);

      brd_status_t s = brd_status(brd, side);
      // TODO: should we take stalemate as a valid move condition?
      if (s != brd_s_normal) continue;

      gai_chk_t chk;
      gai_chk(board, j, &chk);

      unsigned p, n;
      brd_score(brd, &p, &n);
      int score = (int)p - (int)n;
      if (score > score_mx || (score == score_mx && rand() % 2)) {
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
