#ifndef GME_H
#define GME_H

typedef struct gme_state_s {
} gme_state_t;

const gme_state_t * gme_state();

void gme_reset(void);
void gme_tick(void);

void gme_mouse_move(float px, float py);
void gme_mouse_down(void);

#ifdef GME_IMPL

void gme_reset(void) {
}

void gme_tick(void) {
}

void gme_mouse_move(float px, float py) {
}

void gme_mouse_down() {
}

const gme_state_t * gme_state() { return NULL; }

#endif
#endif
