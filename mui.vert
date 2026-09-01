#version 450

struct vtx_t {
  vec4 rect;
  vec4 c0;
  vec4 c1;
  vec4 uv;
  vec2 scr;
};
layout(binding=0) readonly buffer vtx_buf {
  vtx_t vtx[];
}; 

layout(location=0) out vec2 f_uv;
layout(location=1) out vec4 f_c0;
layout(location=2) out vec4 f_c1;

void main() {
  vtx_t v = vtx[gl_InstanceIndex];

  vec2 p = vec2(gl_VertexIndex & 1, (gl_VertexIndex >> 1) & 1);
  f_uv = v.uv.xy + p * v.uv.zw;
  f_c0 = v.c0;
  f_c1 = v.c1;

  p = (v.rect.xy + p * v.rect.zw) / v.scr;
  p = p * 2 - 1;
  gl_Position = vec4(p, 0, 1);
}
