#version 450 core

out vec4 fragColor;

in vec2 texel;

uniform sampler2D hdrBuffer;
uniform sampler2D bloomBuffer;

uniform float exposure;

const float gamma = 2.2f;

vec3 aces(vec3 color) {
	const float a = 2.51;
	const float b = 0.03;
	const float c = 2.43;
	const float d = 0.59;
	const float e = 0.14;
	return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
}

void main() {
	vec3 hdrColor = texture(hdrBuffer, texel).rgb;
	vec3 bloomColor = texture(bloomBuffer, texel).rgb;

	hdrColor += bloomColor;

	vec3 mapped = aces(hdrColor);
	//vec3 mapped = vec3(1.f) - exp(-hdrColor * exposure);

	mapped = pow(mapped, vec3(1 / gamma)) * exposure;

	fragColor = vec4(mapped, 1.f);
}