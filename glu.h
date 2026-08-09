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

#endif
