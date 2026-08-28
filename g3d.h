#ifndef G3D_H
#define G3D_H

typedef void g3d_texture_t;

typedef struct g3d_api_s {
  void * ptr;

  void * (*buffer)(void * ptr, int sz);

  g3d_texture_t * (*texture)(void * ptr, int w, int h);
  void (*load_texture)(g3d_texture_t * txt, void * data);
} g3d_api_t;

#endif
