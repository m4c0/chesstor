#include "brd.h"
#include "mve.h"

static int take_round(char ** line) {
  char * n;
  int res = strtol(*line, &n, 10);
  if (!n || *n != '.') return 0;
  *line = n + 1;
  return res;
}

static inline int adv(char ** line, int n) {
  *line += n;
  if ((*line)[-1] == '+') (*line)++;
  return 1;
}

static inline int is_eom(const char * c) {
  if (*c == '+') c++;
  return *c == 0 || *c == '\n' || *c == ' ';
}
static inline int is_col(char c) {
  return c >= 'a' && c <= 'h';
}
static inline int is_row(char c) {
  return c >= '1' && c <= '8';
}
static inline int is_move(const char * c) {
  return is_col(c[0]) && is_row(c[1]) && is_eom(c + 2);
}

static inline int chartocol(char c) {
  return c - 'a';
}
static inline int chartorow(char c) {
  int y = c - '1';
  return 7 - y;
}
static inline int strtopos(const char * c) {
  return chartorow(c[1]) * 8 + chartocol(c[0]);
}

static int valid(mve_t * mve, unsigned p) {
  if (mve->dir == -1) p |= 0x80;
  if ((mve->board[mve->from] & ~0x40) != p) return 0;
  if (!brd_can_move(mve->board, mve->from, mve->to)) return 0;
  return 1;
}
static int find(mve_t * mve, unsigned p) {
  for (mve->from = 0; mve->from < 8 * 8; mve->from++) {
    if (valid(mve, p)) return 1;
  }
  return 0;
}
static int find_row(mve_t * mve, unsigned p, char row) {
  int r = chartorow(row);
  for (mve->from = 8 * r; mve->from < 8 * r + 8; mve->from++) {
    if (valid(mve, p)) return 1;
  }
  return 0;
}
static int find_col(mve_t * mve, unsigned p, char col) {
  for (mve->from = chartocol(col); mve->from < 8 * 8; mve->from += 8) {
    if (valid(mve, p)) return 1;
  }
  return 0;
}

static int take_move(char ** line, mve_t * mve) {
  char * ptr = *line;
  if (is_move(ptr)) {
    mve->to = strtopos(ptr);
    if (!find(mve, mve_p_pawn)) return 0;
    return adv(line, 3);
  }
  if (is_col(ptr[0]) && ptr[1] == 'x' && is_move(ptr + 2)) {
    mve->to = strtopos(ptr + 2);
    mve->from = (mve->to / 8 - mve->dir) * 8 + chartocol(ptr[0]);
    if (!brd_can_move(mve->board, mve->from, mve->to)) return 0;
    return adv(line, 5);
  }

  if (strncmp(ptr, "O-O", 3) == 0 && is_eom(ptr + 3)) {
    mve->from = strtopos(mve->dir == 1 ? "e8" : "e1");
    mve->to   = strtopos(mve->dir == 1 ? "g8" : "g1");
    if (!valid(mve, mve_p_king)) return 0;
    return adv(line, 4);
  }
  if (strncmp(ptr, "O-O-O", 5) == 0 && is_eom(ptr + 5)) {
    mve->from = strtopos(mve->dir == 1 ? "e8" : "e1");
    mve->to   = strtopos(mve->dir == 1 ? "c8" : "c1");
    if (!valid(mve, mve_p_king)) return 0;
    return adv(line, 6);
  }

  unsigned p = 0;
  if      (*ptr == 'B') p = mve_p_bish;
  else if (*ptr == 'N') p = mve_p_knit;
  else if (*ptr == 'R') p = mve_p_rook;
  else if (*ptr == 'Q') p = mve_p_quen;
  else if (*ptr == 'K') p = mve_p_king;
  else return 0;

  if (is_move(ptr + 1)) {
    mve->to = strtopos(ptr + 1);
    if (!find(mve, p)) return 0;
    return adv(line, 4);
  }
  if (ptr[1] == 'x' && is_move(ptr + 2)) {
    mve->to = strtopos(ptr + 2);
    if (!find(mve, p)) return 0;
    return adv(line, 5);
  }
  if (is_col(ptr[1]) && is_move(ptr + 2)) {
    mve->to = strtopos(ptr + 2);
    if (!find_col(mve, p, ptr[1])) return 0;
    return adv(line, 5);
  }
  if (is_row(ptr[1]) && is_move(ptr + 2)) {
    mve->to = strtopos(ptr + 2);
    if (!find_row(mve, p, ptr[1])) return 0;
    return adv(line, 5);
  }
  if (is_col(ptr[1]) && ptr[2] == 'x' && is_move(ptr + 3)) {
    mve->to = strtopos(ptr + 3);
    if (!find_col(mve, p, ptr[1])) return 0;
    return adv(line, 6);
  }
  if (is_row(ptr[1]) && ptr[2] == 'x' && is_move(ptr + 3)) {
    mve->to = strtopos(ptr + 3);
    if (!find_row(mve, p, ptr[1])) return 0;
    return adv(line, 6);
  }
  return 0;
}

// TODO: check if end-of-game matches the board
static int take_eog(const char * line) {
  if (!line) return 1;
  if (0 == strcmp(line, " 1/2-1/2\n")) return 1;
  if (0 == strcmp(line, " 1-0\n")) return 1;
  if (0 == strcmp(line, " 0-1\n")) return 1;
  return 0;
}
static int take_one_move(char ** line, unsigned * brd, int dir) {
  if (take_eog(*line)) {
    *line = NULL;
    return 0;
  }

  mve_t mve = { .board = brd, .dir = dir };
  if (!take_move(line, &mve)) {
    fprintf(stderr, "invalid move: %s", *line);
    brd_dump(brd);
    return 1;
  }
  printf("  if (opn_mve(%2d, %2d, m)) return;\n", mve.from, mve.to);
  mve_new(&mve, brd, mve.from, mve.to);
  brd_apply(&mve, brd);
  //brd_dump(brd);
  return 0;
}

static int process(char * line, int q) {
  unsigned brd[8 * 8];
  brd_reset(brd);

  // TODO: return movements instead of printing
  printf("static void opn_%d(opn_mve_t * m) {\n", q);
  puts("  brd_reset(m->board);");
  for (int round = 1; line && *line; round++) {
    if (round != take_round(&line)) {
      fprintf(stderr, "invalid round: %s", line);
      return 1;
    }

    if (take_one_move(&line, brd, -1)) return 1;
    if (take_one_move(&line, brd,  1)) return 1;
  }
  puts("}");
  return 0;
}

int main() {
  FILE * f = fopen("test.pgn", "rb");
  
  int n = 0;
  char line[1024];
  while (fgets(line, sizeof(line), f)) {
    if (strncmp(line, "[Result ", 8)) continue;
    // TODO: filter by result? remove '*'?

    while (fgets(line, sizeof(line), f)) {
      if (0 == strcmp(line, "\n")) break;
    }
    if (strcmp(line, "\n")) break;

    if (!fgets(line, sizeof(line), f)) break;
    if (strncmp(line, "1.", 2)) continue;

    if (process(line, n++)) return 1;
  }

  puts("static opn_fn_t opn_fns[] = {");
  for (int i = 0; i < n; i++) printf("  opn_%d,\n", i);
  puts("};");
}
