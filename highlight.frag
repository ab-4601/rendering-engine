#version 450 core

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

void main() {
	fragColor = vec4(1.f, 1.f, 0.f, 1.f);

	float brightness = dot(fragColor.rgb, vec3(0.2125f, 0.7152f, 0.0722f));

	if(brightness > 1.f)
		brightColor = vec4(fragColor.rgb, 1.f);
	else
		brightColor = vec4(0.f, 0.f, 0.f, 1.f);
}