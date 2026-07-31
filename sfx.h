#ifndef SFX_H
#define SFX_H

void sfx_init();
void sfx_filler(float * buf, unsigned sz);

void sfx_move();
void sfx_shuffle();
void sfx_win();

#ifdef SFX_IMPL

#include "gme.h"

#include <math.h>

static float sfx_rand_buf[1024];
// static float sfx_noise(float t) {
//   int i = (int)(440 * t * 1024);
//   return sfx_rand_buf[i % 1024];
// }

//static float sfx_env_note(float ssp) {
//  if (ssp < 0.0) return 0;
//  if (ssp < 0.1) return ssp / 0.1f;
//  if (ssp < 1.0) return 1.0f - (ssp - 0.1f) / 0.9f;
//  return 0;
//}
//static float sfx_env_noise(float ssp) {
//  return 0.0 < ssp && ssp < 1.0 ? 1.0 : 0.0;
//}

// static const float sfx_freqs[] = { 440.f, 523.25f, 392.f, 329.63f };
static int sfx_smp = 0;
void sfx_filler(float * buf, unsigned sz) {
  for (unsigned i = 0; i < sz; ++i) {
    // float t = (sfx_smp + i) / 44100.f;

    float acc = 0;
    for (int i = 0; i < 4; i++) {
      // float f = sfx_freqs[i];
      // float note = sin(t * f * 6.28f) * sfx_env_note(t - gme->clicks[i]);
      // float noise = sfx_noise(t) * sfx_env_noise(t - gme->gameover);
      // acc += note * (1 - noise);
    }
    buf[i] = 0.25f * acc;
  }
  sfx_smp += sz;
}

void sfx_init() {
  for (int i = 0; i < 1024; i++) {
    sfx_rand_buf[i] = (float)rand() / (float)RAND_MAX;
  }
}

#endif
#endif
