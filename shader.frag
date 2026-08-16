#version 450

layout(push_constant) uniform upc {
  vec2  aspect;
  float time;
  uint  hover;
  uint  pick;
} pc;

layout(binding = 0) readonly buffer brd {
  uint board[];
};

layout(location=0) in vec2 f_pos;
layout(location=0) out vec4 colour;

vec2 op_rot(vec2 p, float a) {
  return mat2(cos(a), sin(a), -sin(a), cos(a)) * p;
}

float sd_box(vec2 p, vec2 b) {
  vec2 d = abs(p) - b;
  return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
} 
float sd_cross(vec2 p, vec2 b, float r) {
  p = abs(p);
  p = (p.y > p.x) ? p.yx : p.xy;

  vec2  q = p - b;
  float k = max(q.y, q.x);
  vec2  w = (k > 0.0) ? q : vec2(b.y - p.x, -k);
  return sign(k) * length(max(w, 0.0)) + r;
}
float sd_trapezoid(vec2 p, float r1, float r2, float he) {
  vec2 k1 = vec2(r2, he);
  vec2 k2 = vec2(r2 - r1, 2.0 * he);
  p.x = abs(p.x);
  vec2 ca = vec2(p.x - min(p.x, (p.y < 0.0) ? r1 : r2), abs(p.y) - he);
  vec2 cb = p - k1 + k2 * clamp(dot(k1 - p, k2) / dot(k2, k2), 0.0, 1.0);
  float s = (cb.x < 0.0 && ca.y < 0.0) ? -1.0 : 1.0;
  return s * sqrt(min(dot(ca, ca), dot(cb, cb)));
}
float sd_uneven_capsule(vec2 p, float r1, float r2, float h) {
  p.x = abs(p.x);
  float b = (r1 - r2) / h;
  float a = sqrt(1.0 - b * b);
  float k = dot(p, vec2(-b, a));
  if (k < 0.0) return length(p) - r1;
  if (k > a * h) return length(p - vec2(0.0, h)) - r2;
  return dot(p, vec2(a, b)) - r1;
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

vec3 c_piece_part(vec3 c, bool i, float d) {
  vec3 cc = i ? vec3(1) : vec3(0);
  vec3 cb = i ? vec3(0) : vec3(1.0);
  c = mix(cc, c, step(0, d));
  c = mix(cb, c, smoothstep(0, 0.03, abs(d)));
  return c;
}
vec3 c_piece(vec2 p, uint piece, vec3 c) {
  bool i = (piece & 0x80) == 0x80;
  switch (piece & 0xf) {
    case 0: break;
    case 1: {
      c = c_piece_part(c, i, sd_trapezoid(p, 0.1, 0.3, 0.4));
      c = c_piece_part(c, i, length(p + vec2(0, 0.25)) - 0.3);
      c = c_piece_part(c, i, sd_box(p, vec2(0.2, 0.05)));
      c = c_piece_part(c, i, sd_box(p - vec2(0, 0.45), vec2(0.35, 0.1)));
      break;
    }
    case 2: {
      float d = sd_box(p + vec2(0, 0.5), vec2(0.35, 0.20));
      d = max(d, -sd_box(p + vec2(0, 0.65), vec2(0.1)));

      c = c_piece_part(c, i, sd_trapezoid(p, 0.2, 0.3, 0.4));
      c = c_piece_part(c, i, d);
      c = c_piece_part(c, i, sd_box(p + vec2(0, 0.25), vec2(0.3, 0.05)));
      c = c_piece_part(c, i, sd_box(p - vec2(0, 0.5), vec2(0.35, 0.15)));
      break;
    }
    case 3: {
      p.y *= -1;
      p = p + vec2(0.15, 0.15);

      float d = sd_trapezoid(p + vec2(sin(p.y * 2) * 0.3, 0), 0.5, 0.2, 0.5);
      vec2 nose_p = op_rot(p + vec2(0.15, -0.5), 1.47);
      d = min(d, sd_uneven_capsule(nose_p, 0.3, 0.1, 0.7));
      vec2 mouth_p = op_rot(nose_p + vec2(-0.06, -0.45), 0.4);
      d = min(d, sd_uneven_capsule(mouth_p, 0.1, 0.06, 0.2));
      vec2 ear_p = op_rot(p + vec2(0.26, -0.6), -0.97);
      d = min(d, sd_uneven_capsule(ear_p, 0.15, 0.01, 0.3));
      c = c_piece_part(c, i, d);

      d = length(p + vec2(0.0, -0.6)) - 0.02;
      c = c_piece_part(c, i, d);

      break;
    }
    case 4: {
      p.y = p.y - 0.05;

      vec2 bp = vec2(p.x, -p.y);
      float d = sd_uneven_capsule(bp, 0.4, 0.03, 0.7);
      bp = op_rot(bp + vec2(0, -0.2), 0.3);
      d = max(d, -sd_uneven_capsule(bp, 0.04, 0.03, 0.6));
      c = c_piece_part(c, i, d);

      c = c_piece_part(c, i, sd_trapezoid(p + vec2(0, -0.32), 0.2, 0.3, 0.1));
      c = c_piece_part(c, i, sd_box(p - vec2(0, 0.25), vec2(0.35, 0.05)));
      c = c_piece_part(c, i, sd_box(p - vec2(0, 0.5), vec2(0.4, 0.10)));
      break;
    }
    case 5: {
      vec2 hp = vec2(abs(p.x), p.y);

      c = c_piece_part(c, i, length(p + vec2(0, 0.1)) - 0.4);

      float d = sd_trapezoid(p + vec2(0, 0), 0.6, 0.4, 0.3);
      d = max(d, -(length(hp + vec2(-0.25, 0.3)) - 0.2));
      c = c_piece_part(c, i, d);
      c = c_piece_part(c, i, sd_box(p - vec2(0, 0.5), vec2(0.5, 0.2)));
      c = c_piece_part(c, i, length(hp - vec2(0.13, 0.5)) - 0.05);
      c = c_piece_part(c, i, length(hp - vec2(0.36, 0.5)) - 0.04);
      break;
    }
    case 6: {
      vec2 hp = vec2(abs(p.x), p.y);

      float d = length(p + vec2(0, 0.05)) - 0.35;
      d = min(d, sd_trapezoid(p - vec2(0, 0.05), 0.5, 0.4, 0.25));
      c = c_piece_part(c, i, d);
      c = c_piece_part(c, i, sd_box(p - vec2(0, 0.5), vec2(0.5, 0.2)));
      c = c_piece_part(c, i, length(hp - vec2(0.13, 0.5)) - 0.05);
      c = c_piece_part(c, i, length(hp - vec2(0.36, 0.5)) - 0.04);
      c = c_piece_part(c, i, sd_cross(p + vec2(0.0, 0.55), vec2(0.2, 0.1), 0.05));
      break;
    }
    default: {
      float d = length(p) - 0.5;
      c = c_piece_part(c, i, d); 
      break;
    }
  }
  return c;
}
vec3 c_sqr(vec2 p) {
  p = p * 0.5 + 0.5;
  p = p * 8;

  bool valid = p.x >= 0 && p.x < 8 && p.y >= 0 && p.y < 8;

  vec2 fp = floor(p);
  uint id = uint(p.y) * 8 + uint(p.x);
  p = fract(p);
  p = p * 2 - 1;

  float s = mod(fp.x + fp.y, 2);
  vec3 c = mix(vec3(0.44, 0.47, 0.6), vec3(0.1, 0.15, 0.3), s);
  if (valid && id == pc.hover) {
    float d = sd_box(p, vec2(0.8));
    d = abs(d);
    d = smoothstep(0.05, 0.1, d);
    c = mix(c, vec3(0.8, 0.2, 0.1), d);
  }
  if (valid && id == pc.pick) {
    p /= 0.7;
  }
  if (valid) return c_piece(p, board[id], c);
  return c;
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

