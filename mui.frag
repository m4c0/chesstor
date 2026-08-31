#version 450

layout(binding=1) uniform sampler2D u_txt;

layout(location=0) in vec2 f_uv;
layout(location=1) in vec4 f_colour;
layout(location=0) out vec4 colour;

void main() {
  colour = f_colour * texture(u_txt, f_uv).rrrr;
}

