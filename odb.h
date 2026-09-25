#ifndef ODB_H
#define ODB_H

int odb_init(void);
uint32_t odb_reset(void);
uint32_t odb_pick(uint32_t ofs, unsigned * board, unsigned * from, unsigned * to);

#ifdef ODB_IMPL

int odb_init(void) {
  return 0;
}
uint32_t odb_reset(void) {
  return 0;
}
uint32_t odb_pick(uint32_t ofs, unsigned * board, unsigned * from, unsigned * to) {
  return 0;
}

#endif
#endif
