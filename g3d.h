#ifndef G3D_H
#define G3D_H

typedef void g3d_buffer_t;
typedef void g3d_pipeline_t;
typedef void g3d_sampler_t;
typedef void g3d_texture_t;

typedef struct g3d_api_s {
  void * ptr;

  g3d_buffer_t * (*new_buffer)(void * ptr, int sz);
  void * (*map_buffer)(g3d_buffer_t * buf);

  g3d_sampler_t * (*new_sampler)(void * ptr);

  g3d_texture_t * (*new_texture)(void * ptr, int w, int h);
  void (*load_texture)(g3d_texture_t * txt, void * data);

  g3d_pipeline_t * (*new_pipeline)(void * ptr, const char * shader, unsigned bufs, unsigned txts);
} g3d_api_t;

#endif
