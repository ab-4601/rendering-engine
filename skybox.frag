#version 450 core

out vec4 fragColor;

in vec3 texCoords;

uniform samplerCube skybox;

void main() {
	fragColor = pow(texture(skybox, texCoords), vec4(2.2f));
}