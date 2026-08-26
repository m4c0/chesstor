#version 450

layout(push_constant) uniform upc {
  ivec2 cursor;
} pc;

layout(binding = 0) uniform sampler2D atlas;

layout(location = 0) in vec2 f_pos;

layout(location = 0) out vec4 colour;

float sd_box(vec2 p, vec2 b) {
  vec2 d = abs(p) - b;
  return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
} 
void main() {
  vec2 pp = f_pos * vec2(256, 128);
  float d = sd_box(fract(pp/4) * 2 - 1, vec2(0.5));

  float r = texture(atlas, f_pos).r;
  ivec2 p = ivec2(pp);
  float g = p == pc.cursor ? 1-r : r;
  vec3 c = vec3(r, g, r);

  c = mix(c, vec3(1), smoothstep(0, 0.9, d));

  colour = vec4(c, 1);
}
