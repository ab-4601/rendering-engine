#version 450 core

in vec4 color;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

void main() {
	fragColor = color;

	float brightness = dot(color.rgb, vec3(0.2125f, 0.7152f, 0.0722f));

	if(brightness > 0.5f)
		brightColor = vec4(color.rgb, 1.f);
	else
		brightColor = vec4(0.f, 0.f, 0.f, 1.f);
}