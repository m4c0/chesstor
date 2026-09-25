#ifndef ODB_H
#define ODB_H

#include "g3d.h"

int odb_init(const g3d_api_t * api);
uint32_t odb_reset(void);
uint32_t odb_pick(uint32_t ofs, const unsigned * board, unsigned * from, unsigned * to);

#ifdef ODB_IMPL

static const void * odb_data;
static unsigned odb_size;

int odb_init(const g3d_api_t * api) {
  return 0;
}
uint32_t odb_reset(void) {
  return 0;
}
uint32_t odb_pick(uint32_t ofs, const unsigned * board, unsigned * from, unsigned * to) {
  return 0;
}

#endif
#endif
