#ifndef MUI_H
#define MUI_H

#include "g3d.h"

int mui_init(const g3d_api_t * api);
void mui_frame(const g3d_frame_api_t * api, float scr_w, float scr_h);

#ifdef MUI_IMPL

#include "gme.h"

#define MUI_MAX_QUADS 1024
#define MUI_SH 160

#define MUI_ATLAS_W 256
#define MUI_ATLAS_H 128

#define U(x) ((x) / (float)MUI_ATLAS_W)
#define V(x) ((x) / (float)MUI_ATLAS_H)

typedef struct mui_quad_s {
  float x, y, w, h;
  float ux, uy, uw, uh;
  unsigned c0, c1;
  float scr_w, scr_h;
} mui_quad_t;

static g3d_buffer_t   * mui_buffer;
static g3d_sampler_t  * mui_sampler;
static g3d_texture_t  * mui_texture;
static g3d_pipeline_t * mui_pipeline;

static int mui_loaded = 0;

int mui_init(const g3d_api_t * api) {
  mui_buffer   = api->new_buffer(api->ptr, sizeof(mui_quad_t) * MUI_MAX_QUADS);
  mui_sampler  = api->new_sampler(api->ptr, 1);
  mui_texture  = api->new_texture(api->ptr, MUI_ATLAS_W, MUI_ATLAS_H);
  mui_pipeline = api->new_pipeline(api->ptr, "mui", 1, 1);
  return mui_pipeline ? 0 : 1;
}

static mui_quad_t mui_quads[MUI_MAX_QUADS];
static mui_quad_t * mui_cur_quad;
static float mui_scr_w, mui_scr_h;

static void mui_draw_str(const char * str, float x, float y, unsigned c0, unsigned c1) {
  for (const char * c = str; *c; c++) {
    unsigned cc = *c - 32;
    unsigned u = 8 * (cc / 16);
    unsigned v = 8 * (cc % 16);
    *mui_cur_quad++ = (mui_quad_t) {
      x, y, 8, 8,
      U(u), V(v + 0.5), U(8), V(8),
      c0, c1,
      mui_scr_w, mui_scr_h,
    };
    x += 6;
  }
}

static const char * mui_turn_text(const gme_state_t * gme) {
  switch (gme->status) {
    case gme_s_normal:    return "Turn";
    case gme_s_check:     return "Check";
    case gme_s_checkmate: return "Checkmate";
  }
}
static void mui_draw_turn() {
  const gme_state_t * gme = gme_state();
  if (!gme->side) return;

  unsigned c0 = gme->side != 1 ? 0xCCDDEEFF : 0x112233FF;
  unsigned c1 = gme->side == 1 ? 0xCCDDEEFF : 0x112233FF;

  const char * txt = mui_turn_text(gme);

  float x = (mui_scr_w - strlen(txt)*6) / 2.0;
  float y = gme->side == 1 ? 4 : mui_scr_h - 12;
  mui_draw_str(txt, x, y, c0, c1);
}

void mui_frame(const g3d_frame_api_t * api, float scr_w, float scr_h) {
  if (!mui_loaded) {
    api->load_texture_file(mui_texture, "atlas", MUI_ATLAS_W, MUI_ATLAS_H);
    mui_loaded = 1;
  }

  mui_cur_quad = mui_quads;
  mui_scr_w = scr_w > scr_h ? MUI_SH * scr_w / scr_h : MUI_SH * scr_w / scr_h;
  mui_scr_h = MUI_SH;

  mui_draw_turn();

  int num_quads = mui_cur_quad - mui_quads;
  api->load_buffer(mui_buffer, mui_quads, sizeof(mui_quad_t) * num_quads);

  g3d_render_t t = {
    .ptr       = api->ptr,
    .pipeline  = mui_pipeline,
    .buffers   = (g3d_buffer_t  * []) { mui_buffer, 0 },
    .samplers  = (g3d_sampler_t * []) { mui_sampler, 0 },
    .textures  = (g3d_texture_t * []) { mui_texture, 0 },
    .instances = num_quads,
  };
  api->render(&t);
}

#endif
#endif
