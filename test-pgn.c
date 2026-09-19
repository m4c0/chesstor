#include "brd.h"
#include "mve.h"

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

static int find(mve_t * mve, unsigned p) {
  if (mve->dir == -1) p |= 0x80;
  for (int i = 0; i < 8 * 8; i++) {
    if (mve->board[i] != p) continue;
    if (!brd_can_move(mve->board, i, mve->to)) continue;
    mve->from = i;
    return 1;
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
    printf("pawn takes %.2s\n", ptr + 2);
    *line = ptr + 5;
    return 0;
  }
  if (*ptr == 'B' && is_move(ptr + 1)) {
    printf("bishop to %.2s\n", ptr + 1);
    *line = ptr + 4;
    return 0;
  }
  if (*ptr == 'B' && ptr[1] == 'x' && is_move(ptr + 2)) {
    printf("bishop to %.2s\n", ptr + 1);
    *line = ptr + 5;
    return 0;
  }
  if (*ptr == 'N' && is_move(ptr + 1)) {
    printf("knight to %.2s\n", ptr + 1);
    *line = ptr + 4;
    return 0;
  }
  if (*ptr == 'N' && ptr[1] == 'x' && is_move(ptr + 2)) {
    printf("knight to %.2s\n", ptr + 1);
    *line = ptr + 5;
    return 0;
  }
  if (*ptr == 'Q' && is_move(ptr + 1)) {
    printf("queen to %.2s\n", ptr + 1);
    *line = ptr + 4;
    return 0;
  }
  if (*ptr == 'Q' && ptr[1] == 'x' && is_move(ptr + 2)) {
    printf("queen to %.2s\n", ptr + 1);
    *line = ptr + 5;
    return 0;
  }
  if (strncmp(ptr, "O-O", 3) == 0 && is_eom(ptr[3])) {
    puts("castling");
    *line = ptr + 4;
    return 0;
  }
  return 0;
}

static int process(char * line) {
  for (int round = 1; round < 8; round++) {
    if (round != take_round(&line)) {
      fprintf(stderr, "invalid round: %s", line);
      return 1;
    }

    unsigned brd[8 * 8];
    brd_reset(brd);

    mve_t mve = { .board = brd, .dir = -1 };
    if (!take_move(&line, &mve)) {
      fprintf(stderr, "invalid move: %s", line);
      return 1;
    }
    printf("if (opn_mve(%d, %d, m)) return;\n", mve.from, mve.to);
    brd_apply(&mve, brd);

    mve = (mve_t) { .board = brd, .dir = 1 };
    if (!take_move(&line, &mve)) {
      fprintf(stderr, "invalid move: %s", line);
      return 1;
    }
    printf("if (opn_mve(%d, %d, m)) return;\n", mve.from, mve.to);
    brd_apply(&mve, brd);
  }
  puts("done");
  return 0;
}

int main() {
  FILE * f = fopen("test.pgn", "rb");
  
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

    if (process(line)) return 1;
  }
}
