#ifndef BITED_H
#define BITED_H

#include "g3d.h"

#define BTD_W 256
#define BTD_H 128

#define BTD_MULT 4

typedef struct btd_upc {
  int x, y;
} btd_upc_t;
static btd_upc_t * btd_pc;

static uint8_t btd_atlas[BTD_W * BTD_H];

void btd_replace_atlas();

void btd_init(const g3d_api_t * api) {
  btd_pc = api->buffer(api->ptr, sizeof(btd_upc_t));
}

void btd_cursor(int dx, int dy) {
  int x = btd_pc->x + dx;
  if (x >= 0 && x < BTD_W) btd_pc->x = x;

  int y = btd_pc->y + dy;
  if (y >= 0 && y < BTD_H) btd_pc->y = y;
}

void btd_toggle() {
  int i = btd_pc->y * BTD_W + btd_pc->x;
  btd_atlas[i] = btd_atlas[i] ? 0 : 255;
  btd_replace_atlas();
}

void btd_load() {
  FILE * f = fopen("atlas.img", "rb");
  if (f) {
    fread(btd_atlas, BTD_W * BTD_H, 1, f);
    fclose(f);
  }
  btd_replace_atlas();
}
void btd_save() {
  FILE * f = fopen("atlas.img", "wb");
  fwrite(btd_atlas, BTD_W * BTD_H, 1, f);
  fclose(f);
}

#endif
