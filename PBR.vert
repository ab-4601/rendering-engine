#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 texel;
layout (location = 2) in vec3 normal;

out DATA {
    vec3 color;
    vec2 texel;
    vec3 normal;
    vec3 fragPos;
} data_out;

uniform vec3 color;

void main() {
    data_out.color = color;
    data_out.texel = texel;
    data_out.normal = normal;
    data_out.fragPos = aPos;

	gl_Position = vec4(aPos, 1.f);
}