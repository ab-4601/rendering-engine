#version 450 core

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

in vec4 color;

void main() {
	fragColor = color;

	float brightness = dot(color.rgb, vec3(0.2125f, 0.7152f, 0.0722f));

	if(brightness > 1.f)
		brightColor = vec4(color.rgb, 1.f);
	else
		brightColor = vec4(0.f, 0.f, 0.f, 1.f);
}