#ifndef GLU_H
#define GLU_H

#include "g3d.h"
#include "gme.h"
#include "sfx.h"
#include "snd.h"
#include "tim.h"

#define GLU_BUF_SIZE sizeof(gme_board_t)

typedef struct glu_upc_s {
  float aspect_x, aspect_y;
  float mouse_x, mouse_y;
  float time;
  unsigned hover;
  unsigned pick;
} glu_upc_t;

static int glu_scr_w, glu_scr_h;
static float glu_mouse_x, glu_mouse_y;

static g3d_buffer_t   * glu_upc;
static g3d_pipeline_t * glu_ppl;

void g3d_resize(unsigned w, unsigned h) {
  glu_scr_w = w;
  glu_scr_h = h;
}

int g3d_init(const g3d_api_t * api) {
  srand(time(NULL));

  sfx_init();
  snd_init(sfx_filler);

  gme_reset();

  glu_upc = api->new_buffer(api->ptr, sizeof(glu_upc_t));
  glu_ppl = api->new_pipeline(api->ptr, "shader", 2, 0);

  if (!glu_ppl) return 1;
  return 0;
}

void glu_deinit(void) {
  snd_deinit();
}

void glu_load(void * into) {
  memcpy(into, gme_board(), GLU_BUF_SIZE);
}

static inline float glu_aspect_x() {
  float a = (float)glu_scr_w / (float)glu_scr_h;
  return a > 1 ? a : 1;
}
static inline float glu_aspect_y() {
  float a = (float)glu_scr_h / (float)glu_scr_w;
  return a > 1 ? a : 1;
}
void g3d_frame(const g3d_frame_api_t * api) {
  const gme_state_t * gme = gme_state();

  glu_upc_t pc = {
    .aspect_x = glu_aspect_x(),
    .aspect_y = glu_aspect_y(),
    .mouse_x  = glu_mouse_x,
    .mouse_y  = glu_mouse_y,
    .time     = tim_now(),
    .hover    = gme->hover,
    .pick     = gme->pick,
  };
  api->load_buffer(glu_upc, &pc, sizeof(glu_upc_t));

  g3d_render_t t = {
    .ptr      = api->ptr,
    .pipeline = glu_ppl,
    .buffers  = (g3d_buffer_t *[]) { glu_upc, 0 },
    .textures = (g3d_texture_t *[]) { 0 },
    .samplers = (g3d_sampler_t *[]) { 0 },
  };
  api->render(&t);
}

static float glu_mouse(float p, float a) {
  p = p * 2 - 1;
  p *= a;
  return p;
}
void glu_mouse_move(int x, int y) {
  glu_mouse_x = glu_mouse((float)x / (float)glu_scr_w, glu_aspect_x());
  glu_mouse_y = glu_mouse((float)y / (float)glu_scr_h, glu_aspect_y());
  gme_mouse_move(glu_mouse_x, glu_mouse_y);
}
void glu_mouse_down(int x, int y) {
  glu_mouse_move(x, y);
  gme_mouse_down();
}
void glu_mouse_up(int x, int y) {
  glu_mouse_move(x, y);
  gme_mouse_up();
}
void glu_mouse_cancel(int x, int y) {
  glu_mouse_move(x, y);
  gme_mouse_cancel();
}

#endif
