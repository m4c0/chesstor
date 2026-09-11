#ifndef OPN_H
#define OPN_H

void opn_e4_ruy_lopez(unsigned * board, unsigned moves);

#ifdef OPN_IMPL
#include "brd.h"
#include "mve.h"

#define P(x, y) (x + 8 * (8 - y))
#define A(y) P(0, y)
#define B(y) P(1, y)
#define C(y) P(2, y)
#define D(y) P(3, y)
#define E(y) P(4, y)
#define F(y) P(5, y)
#define G(y) P(6, y)
#define H(y) P(7, y)

void opn_e4_ruy_lopez(unsigned * board, unsigned moves) {
  brd_reset(board);
  if (!moves) return;

  mve_t mve;
  mve_new(&mve, board, E(2), E(4)); brd_apply(&mve, board); if (!--moves) return;
  mve_new(&mve, board, E(7), E(5)); brd_apply(&mve, board); if (!--moves) return;
  mve_new(&mve, board, G(1), F(3)); brd_apply(&mve, board); if (!--moves) return;
  mve_new(&mve, board, B(8), C(6)); brd_apply(&mve, board); if (!--moves) return;
  mve_new(&mve, board, F(1), B(5)); brd_apply(&mve, board); if (!--moves) return;
}

#endif
#endif
