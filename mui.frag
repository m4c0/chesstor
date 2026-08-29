#version 450

layout(location=0) in vec2 f_pos;
layout(location=0) out vec4 colour;

void main() {
  colour = vec4(1, 1, 1, 0.5);
}

