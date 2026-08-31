#version 450

layout(location=0) in vec2 f_pos;
layout(location=1) in vec4 f_clr;
layout(location=0) out vec4 colour;

void main() {
  colour = f_clr;
}

