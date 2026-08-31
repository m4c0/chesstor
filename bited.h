#ifndef BITED_H
#define BITED_H

#include "g3d.h"

#define BTD_W 256
#define BTD_H 128

#define BTD_MULT 4

void btd_load();

typedef struct btd_upc {
  int x, y;
} btd_upc_t;
static btd_upc_t btd_pc;
static g3d_buffer_t * btd_buf;

static uint8_t btd_atlas[BTD_W * BTD_H];
static g3d_texture_t * btd_smp;
static g3d_texture_t * btd_txt;

static g3d_pipeline_t * btd_ppl;

int g3d_init(const g3d_api_t * api) {
  btd_buf = api->new_buffer(api->ptr, sizeof(btd_upc_t));
  btd_smp = api->new_sampler(api->ptr);
  btd_txt = api->new_texture(api->ptr, BTD_W, BTD_H);
  btd_ppl = api->new_pipeline(api->ptr, "bited", 1, 1);
  btd_load();

  if (!btd_ppl) return 1;
  return 0;
}

void g3d_frame(const g3d_frame_api_t * api) {
  api->load_buffer(btd_buf, &btd_pc, sizeof(btd_upc_t));
  api->load_texture(btd_txt, btd_atlas, BTD_W, BTD_H);

  g3d_render_t t = {
    .ptr       = api->ptr,
    .pipeline  = btd_ppl,
    .buffers   = (g3d_buffer_t *[]) { btd_buf, 0 },
    .textures  = (g3d_texture_t *[]) { btd_txt, 0 },
    .samplers  = (g3d_sampler_t *[]) { btd_smp, 0 },
    .instances = 1,
  };
  api->render(&t);
}

void g3d_resize(unsigned sw, unsigned sh) {
}

void btd_cursor(int dx, int dy) {
  int x = btd_pc.x + dx;
  if (x >= 0 && x < BTD_W) btd_pc.x = x;

  int y = btd_pc.y + dy;
  if (y >= 0 && y < BTD_H) btd_pc.y = y;
}

void btd_toggle() {
  int i = btd_pc.y * BTD_W + btd_pc.x;
  btd_atlas[i] = btd_atlas[i] ? 0 : 255;
}

void btd_load() {
  FILE * f = fopen("atlas.img", "rb");
  if (f) {
    fread(btd_atlas, BTD_W * BTD_H, 1, f);
    fclose(f);
  }
}
void btd_save() {
  FILE * f = fopen("atlas.img", "wb");
  fwrite(btd_atlas, BTD_W * BTD_H, 1, f);
  fclose(f);
}

#endif
