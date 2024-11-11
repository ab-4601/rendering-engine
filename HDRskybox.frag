#version 450 core

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

in vec3 texel;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 sample_spherical_map(vec3 v) {
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));

	uv *= invAtan;
	uv += 0.5f;

	return uv;
}

void main() {
	vec2 uv = sample_spherical_map(normalize(texel));
	vec3 finalColor = texture(equirectangularMap, uv).rgb;

	fragColor = vec4(finalColor, 1.f);

	float brightness = dot(finalColor.rgb, vec3(0.2125f, 0.7152f, 0.0722f));

	if(brightness > 2.f)
		brightColor = vec4(finalColor.rgb, 1.f);
	else
		brightColor = vec4(0.f, 0.f, 0.f, 1.f);
}