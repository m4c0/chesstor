#ifndef GME_H
#define GME_H

enum gme_piece_type_e {
  gme_p_none = 0,
  gme_p_pawn,
  gme_p_towr,
  gme_p_knit,
  gme_p_bish,
  gme_p_quen,
  gme_p_king,
};
typedef struct gme_board_s {
  unsigned data[8 * 8];
} gme_board_t;

typedef struct gme_state_s {
  unsigned hover;
  unsigned pick;
} gme_state_t;

const gme_state_t * gme_state();
const gme_board_t * gme_board();

void gme_reset(void);
void gme_tick(void);

void gme_mouse_move(float px, float py);
void gme_mouse_down(void);

#ifdef GME_IMPL

gme_board_t board;
gme_state_t state;

static unsigned template[8 * 2] = {
  gme_p_towr, gme_p_knit, gme_p_bish, gme_p_quen, gme_p_king, gme_p_bish, gme_p_knit, gme_p_towr,
  gme_p_pawn, gme_p_pawn, gme_p_pawn, gme_p_pawn, gme_p_pawn, gme_p_pawn, gme_p_pawn, gme_p_pawn,
};
void gme_reset(void) {
  for (int i = 0; i < 8 * 8; i++) board.data[i] = 0;

  for (int i = 0; i < 8; i++) {
    board.data[i     ] = template[i    ];
    board.data[i +  8] = template[i + 8];
    board.data[i + 48] = template[i + 8] | 0x80;
    board.data[i + 56] = template[i    ] | 0x80;
  }

  state.hover = -1;
  state.pick = -1;
}

void gme_tick(void) {
}

static int brd_norm(float p) {
  p /= 0.9 - 0.07;
  p = p * 0.5 + 0.5;
  p *= 8;
  return (int)p;
}
static int brd_pos(float px, float py) {
  int bx = brd_norm(px);
  int by = brd_norm(py);
  if (bx < 0) return -1;
  if (by < 0) return -1;
  if (bx > 7) return -1;
  if (by > 7) return -1;
  return by * 8 + bx;
}
void gme_mouse_move(float px, float py) {
  state.hover = brd_pos(px, py);
}

void gme_mouse_down() {
  state.pick = state.hover;
}

const gme_board_t * gme_board() { return &board; }

const gme_state_t * gme_state() { return &state; }

#endif
#endif
