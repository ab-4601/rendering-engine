#version 450 core

layout (location = 0) in vec3 aPos;

out vec4 color;

uniform vec3 vColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.f);

	color = vec4(vColor, 1.f);
}