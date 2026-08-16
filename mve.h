#ifndef MVE_H
#define MVE_H

enum mve_piece_type_e {
  mve_p_none = 0,
  mve_p_pawn,
  mve_p_towr, // TODO: rename to rook
  mve_p_knit,
  mve_p_bish,
  mve_p_quen,
  mve_p_king,
  mve_p_errd = 0xF,
};
int mve_is_valid(unsigned * board, int from, int to);

#define MVE_DIR(x) (((x) & 0x80) ? -1 : 1)

#ifdef MVE_IMPL

#define P(x) ((x) & 0xF)
#define D(x) ((x) & 0x80)

#define MOVED(x) ((x) & 0x40)

#define SIGN(x) ((x) ? (x) >= 0 ? 1 : -1 : 0)

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
    if (P(b) != mve_p_none) return mve_p_errd;
  }

  return mve_piece_after_custom_delta(mve, mve->dx, mve->dy);
}

static int mve_linear_is_valid(const mve_t * mve) {
  int b = mve_piece_after_delta(mve);
  if (P(b) == mve_p_errd) return 0;
  if (P(b) == mve_p_none) return 1;
  if (D(b) != D(mve->piece)) return 1;
  return 0;
}

static int mve_pawn_is_valid(const mve_t * mve) {
  if (mve->dx == 0 && mve->dy == mve->dir) {
    int b = mve_piece_after_delta(mve);
    if (P(b) == mve_p_none) return 1;
    return 0;
  }
  if (mve->dx == 0 && mve->dy == 2 * mve->dir) {
    if (MOVED(mve->piece)) return 0;
    int b = mve_piece_after_delta(mve);
    if (P(b) == mve_p_none) return 1;
    return 0;
  }
  if ((mve->dx == 1 || mve->dx == -1) && mve->dy == mve->dir) {
    int b = mve_piece_after_delta(mve);
    if (P(b) == mve_p_none) return 0;
    if (D(b) == D(mve->piece)) return 0;
    return 1;
  }
  return 0;
}

static int mve_knit_is_valid(const mve_t * mve) {
  int ax = abs(mve->dx);
  int ay = abs(mve->dy);
  if (ax + ay != 3) return 0;
  if (ax != 2 && ay != 2) return 0;

  int b = mve_piece_after_custom_delta(mve, mve->dx, mve->dy);
  if (P(b) == mve_p_errd) return 0;
  if (P(b) == mve_p_none) return 1;
  if (D(b) != D(mve->piece)) return 1;
  return 0;
}

static int mve_towr_is_valid(const mve_t * mve) {
  // TODO: castling
  if (mve->dx != 0 && mve->dy != 0) return 0;
  return mve_linear_is_valid(mve);
}

static int mve_bish_is_valid(const mve_t * mve) {
  if (mve->dx == 0 || mve->dy == 0) return 0;
  return mve_linear_is_valid(mve);
}

static int mve_quen_is_valid(const mve_t * mve) {
  return mve_linear_is_valid(mve);
}

static int mve_king_is_valid(const mve_t * mve) {
  // TODO: castling
  // TODO: block if target leaves game on check
  if (abs(mve->dx) > 1 || abs(mve->dy) > 1) return 0;
  return mve_linear_is_valid(mve);
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
  mve.dir = MVE_DIR(mve.piece);

  if (mve.dx == 0 && mve.dy == 0) return 0;

  switch (P(mve.piece)) {
    case mve_p_pawn: return mve_pawn_is_valid(&mve);
    case mve_p_towr: return mve_towr_is_valid(&mve);
    case mve_p_knit: return mve_knit_is_valid(&mve);
    case mve_p_bish: return mve_bish_is_valid(&mve);
    case mve_p_quen: return mve_quen_is_valid(&mve);
    case mve_p_king: return mve_king_is_valid(&mve);
    default: return 0;
  }
}

#endif
#endif
