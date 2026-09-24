#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct node_s {
  uint16_t ft;
  uint16_t prob;
  struct node_s * child;
  struct node_s * sibling;
} node_t;

static int node_count = 0;
static inline void * alloc_node() {
  node_count++;
  void * res = malloc(sizeof(node_t));
  if (!res) fprintf(stderr, "could not allocate node\n");
  return res;
}

static int process(node_t * parent, char * line) {
  long from = strtol(line, &line, 16);
  if (from < 0 || from >= (8 * 8)) return (fprintf(stderr, "invalid position: %lx\n", from), 1);

  long to = strtol(line, &line, 16);
  if (to < 0 || to >= (8 * 8)) return (fprintf(stderr, "invalid position: %lx\n", to), 1);

  uint16_t ft = from * 64 + to;
  node_t * n = NULL;
  if (parent->child) for (n = parent->child; n; n = n->sibling) if (n->ft == ft) break;

  if (n) {
    if (n->prob < 0xFFFF) n->prob++;
  } else {
    n = alloc_node(); if (!n) return 1;
    *n = (node_t) {
      .ft      = ft,
      .prob    = 1,
      .sibling = parent->child,
    };
    parent->child = n;
  }

  if (*line == '\n') return 0;
  if (*line != ' ') return (fprintf(stderr, "invalid post-move char: [%c]\n", *line), 1);

  return process(n, line + 1);
}

typedef struct dump_s {
  uint16_t ft;
  uint16_t prob;
  uint32_t ofs;
} dump_t;
static int dump(FILE * f, const node_t * n, uint32_t * ofs) {
  int kids = 0;
  for (const node_t * c = n->child; c; c = c->sibling) kids++;
  if (!kids) {
    *ofs = 0;
    return 0;
  }

  uint32_t blk_sz = sizeof(dump_t) * kids;
  dump_t * nodes = malloc(blk_sz);
  if (!nodes) return 1;

  dump_t * p = nodes;
  for (const node_t * c = n->child; c; c = c->sibling, p++) {
    p->ft   = c->ft;
    p->prob = c->prob;
    if (dump(f, c, &p->ofs)) return 1;
  }

  long t = ftell(f);
  if (t < 0) return (fprintf(stderr, "error checking output\n"), 1);
  if (t > 0xFFFFFFFF) return (fprintf(stderr, "file got bigger than 2gb\n"), 1);
  *ofs = t;

  if (!fwrite(&blk_sz, 1, 4, f)) return (fprintf(stderr, "error writing page size\n"), 1);
  if (!fwrite(nodes, 1, blk_sz, f)) return (fprintf(stderr, "error writing page data\n"), 1);

  free(nodes);
  return 0;
}

int main() {
  FILE * f = fopen("pgn-extract.out", "rb");
  if (!f) return (fprintf(stderr, "error reading extracted file\n"), 1);

  node_t * root = alloc_node();

  int ret = 0;
  int count = 0;
  char line[1024];
  while (fgets(line, sizeof(line), f)) {
    if (process(root, line)) {
      ret = 1;
      break;
    }
    count++;
  }
  printf("%d rows, %d nodes in total (%lldmb)\n", count, node_count, (uint64_t)node_count * sizeof(node_t) / (1024L * 1024L));
  fclose(f);
  if (ret) return ret;

  FILE * out = fopen("pgn-dbtest.out", "wb");
  if (!out) return (fprintf(stderr, "error opening output\n"), 1);

  const char signature[] = "XADREZ00";
  if (!fwrite(signature, 1, 8, out)) return (fprintf(stderr, "error writing signature\n"), 1);

  uint32_t ofs;
  if (dump(out, root, &ofs)) return 1;
  if (!fwrite(&ofs, 1, 4, out)) return (fprintf(stderr, "error writing root position\n"), 1);

  fclose(out);

  return 0;
}
