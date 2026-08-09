#ifndef GLU_H
#define GLU_H

#include "gme.h"
#include "sfx.h"
#include "snd.h"
#include "tim.h"

#define GLU_BUF_SIZE sizeof(gme_board_t)

typedef struct glu_upc_s {
  float aspect_x, aspect_y;
  float time;
  unsigned hover;
  unsigned pick;
} glu_upc_t;
static glu_upc_t glu_pc;

static int glu_scr_w;
static int glu_scr_h;

void glu_resize(int w, int h) {
  float a = (float)w / (float)h;
  glu_pc.aspect_x = a > 1 ? a : 1;
  glu_pc.aspect_y = a > 1 ? 1 : (1.0 / a);

  glu_scr_w = w;
  glu_scr_h = h;
}

void glu_init(int w, int h) {
  srand(time(NULL));

  sfx_init();
  snd_init(sfx_filler);

  glu_resize(w, h);
}

void glu_deinit(void) {
  snd_deinit();
}

void glu_frame(void) {
  glu_pc.time = tim_now();

  const gme_state_t * gme = gme_state();
  glu_pc.hover = gme->hover;
  glu_pc.pick  = gme->pick;
}

static float glu_mouse(float p, float a) {
#ifdef __APPLE__
  p *= 2;
#endif
  p = p * 2 - 1;
  p *= a;
  return p;
}
void glu_mouse_move(int x, int y) {
  float px = glu_mouse((float)x / (float)glu_scr_w, glu_pc.aspect_x);
  float py = glu_mouse((float)y / (float)glu_scr_h, glu_pc.aspect_y);
  gme_mouse_move(px, py);
}
void glu_mouse_down(int x, int y) {
  float px = glu_mouse((float)x / (float)glu_scr_w, glu_pc.aspect_x);
  float py = glu_mouse((float)y / (float)glu_scr_h, glu_pc.aspect_y);
  gme_mouse_move(px, py);
  gme_mouse_down();
}

#endif
