#ifndef G3D_H
#define G3D_H

typedef struct g3d_api_s {
  void * ptr;
  void * (*buffer)(void * ptr, int sz);
} g3d_api_t;

#endif
