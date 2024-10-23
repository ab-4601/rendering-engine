#version 450 core

in DATA {
	vec3 color;
    vec2 texel;
    vec3 normal;
    vec3 fragPos;
} data_in[];

out DATA {
    vec3 color;
    vec2 texel;
    vec3 normal;
    vec3 fragPos;
} data_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    data_out.color = data_in[0].color;
	data_out.texel = data_in[0].texel;
	data_out.normal = mat3(transpose(inverse(model))) * data_in[0].normal;
	data_out.fragPos = mat3(model) * data_in[0].fragPos;
	gl_Position = projection * view * model * gl_in[0].gl_Position;
	EmitVertex();

	data_out.color = data_in[1].color;
	data_out.texel = data_in[1].texel;
	data_out.normal = mat3(transpose(inverse(model))) * data_in[1].normal;
	data_out.fragPos = mat3(model) * data_in[1].fragPos;
	gl_Position = projection * view * model * gl_in[1].gl_Position;
	EmitVertex();

	data_out.color = data_in[2].color;
	data_out.texel = data_in[2].texel;
	data_out.normal = mat3(transpose(inverse(model))) * data_in[2].normal;
	data_out.fragPos = mat3(model) * data_in[2].fragPos;
	gl_Position = projection * view * model * gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();
}