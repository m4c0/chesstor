#version 450

layout(push_constant) uniform upc {
  vec2  aspect;
} pc;

layout(location=0) in vec2 f_pos;
layout(location=0) out vec4 colour;

float sd_box(vec2 p, vec2 b) {
  vec2 d = abs(p) - b;
  return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
} 

float sd_board(vec2 p) {
  return sd_box(p, vec2(0.9));
}

vec3 c_bg(vec2 p) {
  return vec3(0.1, 0.06, 0.03);
}

void main() {
  vec2 p = f_pos;

  float d = sd_board(p);

  vec3 c = c_bg(p);
  c = mix(vec3(0.2, 0.18, 0.1), c, step(0, d));

  colour = vec4(c, 1);
}

