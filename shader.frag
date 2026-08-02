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

float hash(vec2 p) {
  p = fract(p * vec2(123.34, 345.45));
  p += dot(p, p + 34.345);
  return fract(p.x * p.y);
}
float noise(vec2 p) {
  vec2 i = floor(p);
  vec2 f = smoothstep(0, 1, fract(p));

  float n00 = hash(i + vec2(0, 0));
  float n01 = hash(i + vec2(0, 1));
  float n10 = hash(i + vec2(1, 0));
  float n11 = hash(i + vec2(1, 1));

  return mix(
      mix(n00, n01, f.y),
      mix(n10, n11, f.y),
      f.x);
}
float perlin(vec2 p) {
  return 0.5 * noise(p)
    + 0.25 * noise(p * 2)
    + 0.125 * noise(p * 4)
    + 0.0625 * noise(p * 8);
}
vec3 c_bg(vec2 p) {
  float n = perlin(p + 6.9);
  float m = sin(p.y * 6 + n * 20);
  m = m * 0.5 + 0.5;

  vec3 c = vec3(0.6);
  c = mix(c, vec3(0.7), smoothstep(0.0, 0.005, m));
  c = mix(c, vec3(0.71), smoothstep(0.1, 0.4, m) * (sin(m * 6.2) * 0.5 + 0.5));
  return c;
}

void main() {
  vec2 p = f_pos;

  float d = sd_board(p) - 0.05;

  vec3 c = c_bg(p);
  c = mix(vec3(0), c, smoothstep(0, 0.02, d) * 0.7 + 0.3);
  c = mix(vec3(0.34, 0.25, 0.1), c, step(0, d));

  d = d + 0.05;
  c = mix(vec3(0.6), c, step(0, d));

  colour = vec4(c, 1);
}

