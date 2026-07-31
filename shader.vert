#version 450

layout(push_constant) uniform upc {
  vec2 aspect;
} pc;

layout(location=0) out vec2 f_pos;

void main() {
  vec2 p = vec2(gl_VertexIndex & 1, (gl_VertexIndex >> 1) & 1) * 3;
  p = p * 2 - 1;
  f_pos = p * pc.aspect;
  gl_Position = vec4(p, 0, 1);
}
