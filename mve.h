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
};
int mve_is_valid(unsigned * board, int from, int to);

#ifdef MVE_IMPL

int mve_is_valid(unsigned * board, int from, int to) {
  int brd = board[from];
  // int blk = 0; // TODO: differ logic to black pieces

  int m = to - from;
  switch (brd & 0xF) {
    case mve_p_pawn:
      if (m == -8) return 1;
      if (m == -16 && (from / 8 == 6)) return 1;
      return 0;
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
