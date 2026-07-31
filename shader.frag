#version 450

layout(push_constant) uniform upc {
  vec2  aspect;
} pc;

layout(location=0) in vec2 f_pos;
layout(location=0) out vec4 colour;

void main() {
  colour = vec4(f_pos, 1, 1);
}

