#version 450

layout(binding=1) uniform sampler2D u_txt;

layout(location=0) in vec2 f_uv;
layout(location=1) in vec4 f_colour;
layout(location=0) out vec4 colour;

void main() {
  float d = texture(u_txt, f_uv).r;
  colour = vec4(1) * step(0.5, d);
  d = abs(d * 2 - 1);
  colour = mix(f_colour, colour, smoothstep(0.1, 0.3, d));
}

