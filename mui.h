#ifndef MUI_H
#define MUI_H

#include "g3d.h"

int mui_init(const g3d_api_t * api);
void mui_frame(const g3d_frame_api_t * api, float scr_w, float scr_h);

#ifdef MUI_IMPL

#define MUI_MAX_QUADS 1024
#define MUI_SH 10

typedef struct mui_quad_s {
  float x, y, w, h;
  float r, g, b, a;
  float scr_w, scr_h;
  float pad[2];
} mui_quad_t;

static g3d_buffer_t   * mui_buffer;
static g3d_sampler_t  * mui_sampler;
static g3d_texture_t  * mui_texture;
static g3d_pipeline_t * mui_pipeline;

int mui_init(const g3d_api_t * api) {
  mui_buffer   = api->new_buffer(api->ptr, sizeof(mui_quad_t) * MUI_MAX_QUADS);
  mui_sampler  = api->new_sampler(api->ptr);
  mui_texture  = api->new_texture(api->ptr, 256, 128);
  mui_pipeline = api->new_pipeline(api->ptr, "mui", 1, 1);
  return mui_pipeline ? 0 : 1;
}

static mui_quad_t mui_quads[MUI_MAX_QUADS];
void mui_frame(const g3d_frame_api_t * api, float scr_w, float scr_h) {
  float sw = scr_w > scr_h ? MUI_SH * scr_w / scr_h : MUI_SH * scr_w / scr_h;
  float sh = MUI_SH;
  int num_quads = 2;
  mui_quads[0] = (mui_quad_t) {
    1, 1, 1, 1,
    0.1, 0.2, 0.3, 1,
    sw, sh,
  };
  mui_quads[1] = (mui_quad_t) {
    2, 2, 5, 4,
    0.1, 0.2, 0.3, 0.5,
    sw, sh,
  };
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
