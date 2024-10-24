#version 450 core

out vec4 fragColor;

in vec2 texel;

uniform sampler2D hdrBuffer;
uniform sampler2D bloomBuffer;

uniform float exposure;

void main() {
	vec3 hdrColor = texture(hdrBuffer, texel).rgb;
	vec3 bloomColor = texture(bloomBuffer, texel).rgb;

	hdrColor += bloomColor;

	vec3 mapped = vec3(1.f) - exp(-hdrColor * exposure);

	fragColor = vec4(mapped, 1.f);
}