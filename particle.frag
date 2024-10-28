#version 450 core

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

in GS_OUT {
	vec4 color;
	vec2 texCoord1;
	vec2 texCoord2;
	float blend;
} fs_in;

uniform sampler2D textureSampler;

void main() {
	//fragColor = fs_in.color;
	vec4 texColor1 = texture2D(textureSampler, fs_in.texCoord1);
	vec4 texColor2 = texture2D(textureSampler, fs_in.texCoord2);

	vec4 finalColor = mix(texColor1, texColor2, fs_in.blend);

	if(finalColor.a < 0.1)
		discard;

	fragColor = pow(finalColor, vec4(2.2f));

	float brightness = dot(finalColor.rgb, vec3(0.2125f, 0.7152f, 0.0722f));

	if(brightness > 0.5f)
		brightColor = vec4(finalColor.rgb, 1.f);
	else
		brightColor = vec4(0.f, 0.f, 0.f, 1.f);
}