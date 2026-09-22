#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct blk_s {
  struct blk_s * ptr;
  unsigned prob;
} blk_t;
#define BLOCK_SIZE (sizeof(blk_t) * (8 * 8) * (8 * 8))

static int block_count = 0;
static inline void * alloc_block() {
  block_count++;
  return calloc(BLOCK_SIZE, 1);
}

static int process(blk_t * node, char * line) {
  long from = strtol(line, &line, 16);
  if (from < 0 || from >= (8 * 8)) return (fprintf(stderr, "invalid position: %lx\n", from), 1);

  long to = strtol(line, &line, 16);
  if (to < 0 || to >= (8 * 8)) return (fprintf(stderr, "invalid position: %lx\n", to), 1);

  blk_t * n = node + from * (8 * 8) + to;
  n->prob++;

  if (!n->ptr) n->ptr = alloc_block();

  return 0;
}

int main() {
  FILE * f = fopen("pgn-extract.out", "rb");
  if (!f) return (fprintf(stderr, "error reading extracted file\n"), 1);

  blk_t * root = alloc_block();

  int count = 0;
  char line[1024];
  while (fgets(line, sizeof(line), f)) {
    if (process(root, line)) return 1;
    count++;
  }
  printf("%d rows, %d blocks in total (%lldmb)\n", count, block_count, (uint64_t)block_count * BLOCK_SIZE / (1024L * 1024L));
  fclose(f);
}
