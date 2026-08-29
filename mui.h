#ifndef MUI_H
#define MUI_H

#include "g3d.h"

#define MUI_MAX_QUADS 1024

int mui_init(const g3d_api_t * api);
void mui_frame(const g3d_frame_api_t * api, unsigned scr_w, unsigned scr_h);

#ifdef MUI_IMPL

typedef struct mui_quad_s {
  float x, y, w, h;
} mui_quad_t;

int mui_init(const g3d_api_t * api) {
  return 0;
}

void mui_frame(const g3d_frame_api_t * api, unsigned scr_w, unsigned scr_h) {
}

#endif
#endif
