#ifndef ODB_H
#define ODB_H

#include "g3d.h"

int odb_init(const g3d_api_t * api);
uint32_t odb_reset(void);
uint32_t odb_pick(uint32_t ofs, const unsigned * board, unsigned * from, unsigned * to);

#ifdef ODB_IMPL

static const char * odb_data;
static unsigned odb_size;

int odb_init(const g3d_api_t * api) {
  odb_data = api->load_resource("pgn-dbtest", "out", &odb_size);

  if (0 != strncmp(odb_data, "XADREZ00", 8)) odb_data = NULL;

  return 0;
}
uint32_t odb_reset(void) {
  return *(const uint32_t *)(odb_data + odb_size - 4);
}

typedef struct odb_node_s {
  uint16_t ft;
  uint16_t prob;
  uint32_t ofs;
} odb_node_t;
uint32_t odb_pick(uint32_t ofs, const unsigned * board, unsigned * from, unsigned * to) {
  return 0;
}

#endif
#endif
