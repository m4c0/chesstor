#version 450

struct vtx_t {
  vec4 rect;
  vec4 uv;
  uint c0;
  uint c1;
  vec2 scr;
};
layout(binding=0) readonly buffer vtx_buf {
  vtx_t vtx[];
}; 

layout(location=0) out vec2 f_uv;
layout(location=1) out vec4 f_c0;
layout(location=2) out vec4 f_c1;

vec4 colour(uint c) {
  uint r = (c >> 24) & 0xFF;
  uint g = (c >> 16) & 0xFF;
  uint b = (c >>  8) & 0xFF;
  uint a = (c      ) & 0xFF;
  return vec4(r, g, b, a) / 255.f;
}
void main() {
  vtx_t v = vtx[gl_InstanceIndex];

  vec2 p = vec2(gl_VertexIndex & 1, (gl_VertexIndex >> 1) & 1);
  f_uv = v.uv.xy + p * v.uv.zw;
  f_c0 = colour(v.c0);
  f_c1 = colour(v.c1);

  p = (v.rect.xy + p * v.rect.zw) / v.scr;
  p = p * 2 - 1;
  gl_Position = vec4(p, 0, 1);
}
