#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int take_round(char ** line) {
  char * n;
  int res = strtol(*line, &n, 10);
  if (!n || *n != '.') return 0;
  *line = n + 1;
  return res;
}

static int process(char * line) {
  if (1 != take_round(&line)) {
    fprintf(stderr, "invalid round: %s", line);
    return 1;
  }

  puts(line);
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
