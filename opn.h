#ifndef OPN_H
#define OPN_H

typedef struct opn_mve_s {
  unsigned board[8 * 8];
  unsigned from, to;
  unsigned moves;
} opn_mve_t;

void opn_init();

const opn_mve_t * opn_pick(const unsigned * board);

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

static inline int opn_mve(unsigned from, unsigned to, opn_mve_t * m) {
  if (!m->moves) {
    m->from = from;
    m->to = to;
    return 1;
  }

  mve_t mve;
  mve_new(&mve, m->board, from, to);
  brd_apply(&mve, m->board);

  (m->moves)--;
  return 0;
}

static void opn_e4_italian(opn_mve_t * m) {
  brd_reset(m->board);
  if (opn_mve(E(2), E(4), m)) return;
  if (opn_mve(E(7), E(5), m)) return;
  if (opn_mve(G(1), F(3), m)) return;
  if (opn_mve(B(8), C(6), m)) return;
  if (opn_mve(F(1), C(4), m)) return;
}

static void opn_e4_ruy_lopez(opn_mve_t * m) {
  brd_reset(m->board);
  if (opn_mve(E(2), E(4), m)) return;
  if (opn_mve(E(7), E(5), m)) return;
  if (opn_mve(G(1), F(3), m)) return;
  if (opn_mve(B(8), C(6), m)) return;
  if (opn_mve(F(1), B(5), m)) return;
}

static void opn_e4_scotch(opn_mve_t * m) {
  brd_reset(m->board);
  if (opn_mve(E(2), E(4), m)) return;
  if (opn_mve(E(7), E(5), m)) return;
  if (opn_mve(G(1), F(3), m)) return;
  if (opn_mve(B(8), C(6), m)) return;
  if (opn_mve(D(2), D(4), m)) return;
}

static void opn_e4_sicilian(opn_mve_t * m) {
  brd_reset(m->board);
  if (opn_mve(E(2), E(4), m)) return;
  if (opn_mve(C(7), C(5), m)) return;
}

typedef void (*opn_fn_t)(opn_mve_t *);
static opn_fn_t opn_fns[] = {
  opn_e4_italian,
  opn_e4_ruy_lopez,
  opn_e4_scotch,
  opn_e4_sicilian,
};
#define opn_fn_sz (sizeof(opn_fns) / sizeof(opn_fns[0]))

static opn_mve_t opn_cache[opn_fn_sz * 8];

void opn_init() {
  opn_mve_t * m = opn_cache;
  for (int i = 0; i < opn_fn_sz; i++) {
    for (int j = 0; j < 8; j++, m++) {
      m->moves = j;
      opn_fns[i](m);
      if (!m->from && !m->to) break;
    }
  }
}

const opn_mve_t * opn_pick(const unsigned * board) {
  const opn_mve_t * res = NULL;

  for (const opn_mve_t * m = opn_cache; m->from && m->to; m++) {
    // TODO: use a random tie breaker
    if (0 == memcmp(board, m->board, 8 * 8 * 4)) res = m;
  }

  return res;
}

#endif
#endif
