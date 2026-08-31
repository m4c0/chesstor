#version 450

struct vtx_t {
  vec4 rect;
  vec4 colour;
  vec2 scr;
};
layout(binding=0) readonly buffer vtx_buf {
  vtx_t vtx[];
}; 

layout(location=0) out vec2 f_pos;
layout(location=1) out vec4 f_clr;

void main() {
  vtx_t v = vtx[gl_InstanceIndex];

  vec2 p = vec2(gl_VertexIndex & 1, (gl_VertexIndex >> 1) & 1);
  f_pos = p * 2 - 1;
  f_clr = v.colour;

  p = (v.rect.xy + p * v.rect.zw) / v.scr;
  p = p * 2 - 1;
  gl_Position = vec4(p, 0, 1);
}
