#ifndef OPN_H
#define OPN_H

typedef struct opn_mve_s {
  unsigned from, to;
  unsigned moves;
} opn_mve_t;

void opn_e4_italian  (unsigned * board, opn_mve_t * m);
void opn_e4_ruy_lopez(unsigned * board, opn_mve_t * m);
void opn_e4_scotch   (unsigned * board, opn_mve_t * m);
void opn_e4_sicilian (unsigned * board, opn_mve_t * m);

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

static inline int opn_mve(unsigned * board, unsigned from, unsigned to, opn_mve_t * m) {
  if (!m->moves) {
    m->from = from;
    m->to = to;
    return 1;
  }

  mve_t mve;
  mve_new(&mve, board, from, to);
  brd_apply(&mve, board);

  (m->moves)--;
  return 0;
}

void opn_e4_italian(unsigned * board, opn_mve_t * m) {
  brd_reset(board);

  if (opn_mve(board, E(2), E(4), m)) return;
  if (opn_mve(board, E(7), E(5), m)) return;
  if (opn_mve(board, G(1), F(3), m)) return;
  if (opn_mve(board, B(8), C(6), m)) return;
  if (opn_mve(board, F(1), C(4), m)) return;
}

void opn_e4_ruy_lopez(unsigned * board, opn_mve_t * m) {
  brd_reset(board);

  if (opn_mve(board, E(2), E(4), m)) return;
  if (opn_mve(board, E(7), E(5), m)) return;
  if (opn_mve(board, G(1), F(3), m)) return;
  if (opn_mve(board, B(8), C(6), m)) return;
  if (opn_mve(board, F(1), B(5), m)) return;
}

void opn_e4_scotch(unsigned * board, opn_mve_t * m) {
  brd_reset(board);

  if (opn_mve(board, E(2), E(4), m)) return;
  if (opn_mve(board, E(7), E(5), m)) return;
  if (opn_mve(board, G(1), F(3), m)) return;
  if (opn_mve(board, B(8), C(6), m)) return;
  if (opn_mve(board, D(2), D(4), m)) return;
}

void opn_e4_sicilian(unsigned * board, opn_mve_t * m) {
  brd_reset(board);

  if (opn_mve(board, E(2), E(4), m)) return;
  if (opn_mve(board, C(7), C(5), m)) return;
}

#endif
#endif
