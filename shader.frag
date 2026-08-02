#version 450

layout(push_constant) uniform upc {
  vec2  aspect;
} pc;

layout(binding = 0) readonly buffer brd {
  uint board[];
};

layout(location=0) in vec2 f_pos;
layout(location=0) out vec4 colour;

float sd_box(vec2 p, vec2 b) {
  vec2 d = abs(p) - b;
  return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
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
vec3 c_back(vec2 p) {
  float n = perlin(p + 6.9);
  float m = sin(p.y * 6 + n * 20);
  m = m * 0.5 + 0.5;

  vec3 c = vec3(0.6);
  c = mix(c, vec3(0.7), smoothstep(0.0, 0.005, m));
  c = mix(c, vec3(0.71), smoothstep(0.1, 0.4, m) * (sin(m * 6.2) * 0.5 + 0.5));
  return c;
}

vec3 c_wood(vec2 p) {
  float n = perlin(p + 6.9);
  float m = sin(p.y * 26 + n * 15);
  m = m * 0.5 + 0.5;

  vec3 c = vec3(0.34, 0.2, 0.01);
  c = mix(c, vec3(0.5, 0.25, 0.05), smoothstep(0.01, 0.005, m));
  c = mix(c, vec3(0.56, 0.32, 0.06) * 0.9, smoothstep(0.1, 0.4, m) * (sin(m * 36.2) * 0.5 + 0.5));
  return c;
}
vec3 c_border(vec2 p, vec3 c) {
  float d = sd_box(p, vec2(0.9));
  d = abs(d) - 0.05;

  c = mix(vec3(0), c, smoothstep(0, 0.02, d) * 0.7 + 0.3);
  c = mix(c_wood(p), c, step(0, d));
  return c;
}

vec3 c_piece(vec2 p, uint piece, vec3 c) {
  if (piece == 0) return c;

  float d = length(p);

  return mix(vec3(1), c, d);
}
vec3 c_sqr(vec2 p) {
  p = p * 0.5 + 0.5;
  p = p * 8;

  vec2 fp = floor(p);
  uint id = uint(p.y) * 8 + uint(p.x);
  p = fract(p);
  p = p * 2 - 1;

  float s = mod(fp.x + fp.y, 2);
  vec3 c = mix(vec3(0.54, 0.57, 0.6), vec3(0.1, 0.15, 0.2), s);
  return c_piece(p, board[id], c);
}
vec3 c_board(vec2 p, vec3 c) {
  p /= 0.9 - 0.07;
  float d = sd_box(p, vec2(1.05));

  c = mix(c_sqr(p), c, step(0, d));
  return c;
}

void main() {
  vec2 p = f_pos;

  vec3 c = c_back(p);
  c = c_board(p, c);
  c = c_border(p, c);

  colour = vec4(c, 1);
}

