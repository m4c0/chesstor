#ifndef MVE_H
#define MVE_H

enum mve_piece_type_e {
  mve_p_none = 0,
  mve_p_pawn,
  mve_p_towr,
  mve_p_knit,
  mve_p_bish,
  mve_p_quen,
  mve_p_king,
  mve_p_errd = 0xF,
};
int mve_is_valid(unsigned * board, int from, int to);

#ifdef MVE_IMPL

#define P(x) ((x) & 0xF)
#define D(x) ((x) & 0x80)

#define SIGN(x) ((x) >= 0 ? 1 : -1)

typedef struct mve_s {
  unsigned * board;
  unsigned piece;
  int from_x, from_y;
  int to_x, to_y;
  int dx, dy;
  int dir;
} mve_t;

static int mve_piece_after_custom_delta(const mve_t * mve, int dx, int dy) {
  int x = mve->from_x + dx;
  if (x < 0 || x > 7) return mve_p_errd;
  int y = mve->from_y + dy;
  if (y < 0 || y > 7) return mve_p_errd;
  return mve->board[y * 8 + x];
}
static int mve_piece_after_delta(const mve_t * mve) {
  int steps = 0;
  if (mve->dx == 0) steps = abs(mve->dy);
  else if (mve->dy == 0) steps = abs(mve->dx);
  else if (abs(mve->dx) == abs(mve->dy)) steps = abs(mve->dx);

  if (steps == 0) return mve_p_errd;

  for (int i = 1; i < steps; i++) {
    int b = mve_piece_after_custom_delta(mve, SIGN(mve->dx) * i, SIGN(mve->dy) * i);
    if (P(b) != mve_p_none) return 0;
  }

  return mve_piece_after_custom_delta(mve, mve->dx, mve->dy);
}

static int mve_pawn_is_valid(const mve_t * mve) {
  if (mve->dx == 0 && mve->dy == mve->dir) {
    int b = mve_piece_after_delta(mve);
    if (P(b) == mve_p_none) return 1;
    return 0;
  }
  if (mve->dx == 0 && mve->dy == 2 * mve->dir) {
    if (mve->dir == -1 && mve->from_y != 6) return 0;
    if (mve->dir ==  1 && mve->from_y != 1) return 0;
    int b = mve_piece_after_delta(mve);
    if (P(b) == mve_p_none) return 1;
    return 1;
  }
  if ((mve->dx == 1 || mve->dx == -1) && mve->dy == mve->dir) {
    int b = mve_piece_after_delta(mve);
    if (P(b) == mve_p_none) return 0;
    if (D(b) == D(mve->piece)) return 0;
    return 1;
  }
  return 0;
}

int mve_is_valid(unsigned * board, int from, int to) {
  mve_t mve  = {
    .board   = board,
    .piece   = board[from],
    .from_x  = from % 8,
    .from_y  = from / 8,
    .to_x    = to % 8,
    .to_y    = to / 8,
  };
  mve.dx  = mve.to_x - mve.from_x;
  mve.dy  = mve.to_y - mve.from_y;
  mve.dir = D(mve.piece) ? -1 : 1;

  if (mve.dx == 0 && mve.dy == 0) return 0;

  switch (P(mve.piece)) {
    case mve_p_pawn: return mve_pawn_is_valid(&mve);
    case mve_p_towr:
    case mve_p_knit:
    case mve_p_bish:
    case mve_p_quen:
    case mve_p_king:
    default: return 0;
  }
}

#endif
#endif
