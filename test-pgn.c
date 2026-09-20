#include "brd.h"
#include "mve.h"

void dump_board(unsigned * b) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++, b++) {
      printf("%02x ", *b);
    }
    printf("\n");
  }
}

static int take_round(char ** line) {
  char * n;
  int res = strtol(*line, &n, 10);
  if (!n || *n != '.') return 0;
  *line = n + 1;
  return res;
}

static inline int is_eom(char c) {
  return c == 0 || c == '\n' || c == ' ';
}
static inline int is_col(char c) {
  return c >= 'a' && c <= 'h';
}
static inline int is_row(char c) {
  return c >= '1' && c <= '8';
}
static inline int is_move(const char * c) {
  return is_col(c[0]) && is_row(c[1]) && is_eom(c[2]);
}

static inline int strtopos(const char * c) {
  int x = c[0] - 'a';
  int y = c[1] - '1';
  return (7 - y) * 8 + x;
}

static int valid(mve_t * mve, unsigned p) {
  if (mve->dir == -1) p |= 0x80;
  if (mve->board[mve->from] != p) return 0;
  if (!brd_can_move(mve->board, mve->from, mve->to)) return 0;
  return 1;
}
static int find(mve_t * mve, unsigned p) {
  for (int i = 0; i < 8 * 8; i++) {
    mve->from = i;
    if (valid(mve, p)) return 1;
    if (valid(mve, p | 0x40)) return 1;
  }
  return 0;
}

static int take_move(char ** line, mve_t * mve) {
  char * ptr = *line;
  if (is_move(ptr)) {
    mve->to = strtopos(ptr);
    if (!find(mve, mve_p_pawn)) return 0;
    *line = ptr + 3;
    return 1;
  }
  if (is_col(ptr[0]) && ptr[1] == 'x' && is_move(ptr + 2)) {
    mve->to = strtopos(ptr + 2);
    mve->from = (mve->to / 8 - mve->dir) * 8 + ptr[0] - 'a';
    if (!brd_can_move(mve->board, mve->from, mve->to)) return 0;
    *line = ptr + 5;
    return 1;
  }

  if (strncmp(ptr, "O-O", 3) == 0 && is_eom(ptr[3])) {
    mve->from = strtopos(mve->dir == 1 ? "e8" : "e1");
    mve->to   = strtopos(mve->dir == 1 ? "g8" : "g1");
    if (!valid(mve, mve_p_king)) return 0;
    *line = ptr + 4;
    return 1;
  }
  if (strncmp(ptr, "O-O-O", 5) == 0 && is_eom(ptr[5])) {
    mve->from = strtopos(mve->dir == 1 ? "e8" : "e1");
    mve->to   = strtopos(mve->dir == 1 ? "c8" : "c1");
    if (!valid(mve, mve_p_king)) return 0;
    *line = ptr + 6;
    return 1;
  }

  unsigned p = 0;
  if      (*ptr == 'B') p = mve_p_bish;
  else if (*ptr == 'N') p = mve_p_knit;
  else if (*ptr == 'Q') p = mve_p_quen;
  else if (*ptr == 'K') p = mve_p_king;
  else return 0;

  if (is_move(ptr + 1)) {
    mve->to = strtopos(ptr + 1);
    if (!find(mve, p)) return 0;
    *line = ptr + 4;
    return 1;
  }
  if (ptr[1] == 'x' && is_move(ptr + 2)) {
    mve->to = strtopos(ptr + 2);
    if (!find(mve, p)) return 0;
    *line = ptr + 5;
    return 1;
  }
  return 0;
}

static int take_one_move(char ** line, unsigned * brd, int dir) {
  mve_t mve = { .board = brd, .dir = dir };
  if (!take_move(line, &mve)) {
    fprintf(stderr, "invalid move: %s", *line);
    return 1;
  }
  printf("  if (opn_mve(%2d, %2d, m)) return;\n", mve.from, mve.to);
  mve_new(&mve, brd, mve.from, mve.to);
  brd_apply(&mve, brd);
  //dump_board(brd);
  return 0;
}

static int process(char * line, int q) {
  unsigned brd[8 * 8];
  brd_reset(brd);

  printf("static void opn_%d(opn_mve_t * m) {\n", q);
  puts("  brd_reset(m->board);");
  for (int round = 1; round < 8; round++) {
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
