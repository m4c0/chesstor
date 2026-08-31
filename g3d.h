#ifndef G3D_H
#define G3D_H

typedef void g3d_buffer_t;
typedef void g3d_pipeline_t;
typedef void g3d_sampler_t;
typedef void g3d_texture_t;

typedef struct g3d_api_s {
  void * ptr;
  g3d_buffer_t * (*new_buffer)(void * ptr, int sz);
  g3d_sampler_t * (*new_sampler)(void * ptr);
  g3d_texture_t * (*new_texture)(void * ptr, int w, int h);
  g3d_pipeline_t * (*new_pipeline)(void * ptr, const char * shader, unsigned bufs, unsigned txts);
} g3d_api_t;

typedef struct g3d_render_s {
  void * ptr;
  g3d_pipeline_t * pipeline;
  g3d_buffer_t ** buffers;
  g3d_texture_t ** textures;
  g3d_sampler_t ** samplers;
  unsigned instances;
} g3d_render_t;
typedef struct g3d_frame_api_s {
  void * ptr;
  void (*load_buffer)(g3d_buffer_t * buf, const void * data, unsigned sz);
  void (*load_texture)(g3d_texture_t * txt, const void * data, unsigned w, unsigned h);
  void (*load_texture_file)(g3d_texture_t * txt, const char * name, unsigned w, unsigned h);
  void (*render)(const g3d_render_t * t);
} g3d_frame_api_t;

int g3d_init(const g3d_api_t * t);
void g3d_frame(const g3d_frame_api_t * t);
void g3d_resize(unsigned sw, unsigned sh);

#endif
