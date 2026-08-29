#ifndef MUI_H
#define MUI_H

#include "g3d.h"

int mui_init(const g3d_api_t * api);
void mui_frame(const g3d_frame_api_t * api, unsigned scr_w, unsigned scr_h);

#ifdef MUI_IMPL

#define MUI_MAX_QUADS 1024

typedef struct mui_quad_s {
  float x, y, w, h;
} mui_quad_t;

static g3d_buffer_t * mui_quads;
static g3d_texture_t * mui_texture;
static g3d_pipeline_t * mui_pipeline;

int mui_init(const g3d_api_t * api) {
  mui_quads = api->new_buffer(api->ptr, sizeof(mui_quad_t) * MUI_MAX_QUADS);
  mui_texture = api->new_texture(api->ptr, 256, 128);
  mui_pipeline = api->new_pipeline(api->ptr, "mui", 1, 1);
  return mui_pipeline ? 0 : 1;
}

void mui_frame(const g3d_frame_api_t * api, unsigned scr_w, unsigned scr_h) {
}

#endif
#endif
