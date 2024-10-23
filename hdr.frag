#version 450 core

out vec4 fragColor;
in vec2 texel;

uniform sampler2D hdrBuffer;

void main() {
	vec3 hdrColor = texture(hdrBuffer, texel).rgb;
	fragColor = vec4(hdrColor, 1.f);
}