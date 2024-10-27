#version 450 core

out vec4 fragColor;
in vec3 texel;

uniform sampler2D equirectangularMap;

const vec2 invATan = vec2(0.1591, 0.3183);

vec2 sample_spherical_map(vec3 v) {
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));

	uv *= invATan;
	uv += 0.5;

	return uv;
}

void main() {
	vec2 uv = sample_spherical_map(normalize(texel));
	vec3 color = texture(equirectangularMap, uv).rgb;

	fragColor = vec4(color, 1.f);
}