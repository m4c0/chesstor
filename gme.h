#ifndef GME_H
#define GME_H

typedef struct gme_board_s {
  float dummy;
} gme_board_t;

typedef struct gme_state_s {
} gme_state_t;

const gme_state_t * gme_state();
const gme_board_t * gme_board();

void gme_reset(void);
void gme_tick(void);

void gme_mouse_move(float px, float py);
void gme_mouse_down(void);

#ifdef GME_IMPL

gme_board_t board;

void gme_reset(void) {
}

void gme_tick(void) {
}

void gme_mouse_move(float px, float py) {
}

void gme_mouse_down() {
}

const gme_board_t * gme_board() { return &board; }

const gme_state_t * gme_state() { return NULL; }

#endif
#endif
