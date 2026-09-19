#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int take_round(const char ** line) {
  int res = atoi(*line);
  return 0;
}

static int process(const char * line) {
  if (1 != take_round(&line)) {
    fprintf(stderr, "invalid round: %s", line);
    return 1;
  }

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
