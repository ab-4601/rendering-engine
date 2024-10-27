#version 450 core

out vec4 fragColor;

in vec3 texel;

uniform samplerCube skybox;

void main() {
	fragColor = pow(texture(skybox, texel), vec4(2.2f));
}