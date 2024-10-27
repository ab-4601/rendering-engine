#version 450 core

layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 3 ) out;

in DATA {
	vec4 vColor;
    vec2 texCoord;
    vec3 normal;
	vec3 tangent;
    vec4 fragPos;
} data_in[];

out DATA {
	vec4 vColor;
    vec2 texCoord;
    vec3 normal;
	vec3 tangent;
    vec4 fragPos;
} data_out;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main() {
	data_out.vColor = data_in[0].vColor;
	data_out.texCoord = data_in[0].texCoord;
	data_out.fragPos = model * data_in[0].fragPos;
	data_out.normal = mat3(transpose(inverse(model))) * data_in[0].normal;
	data_out.tangent = mat3(transpose(inverse(model))) * data_in[0].tangent;
	gl_Position = projection * view * model * gl_in[0].gl_Position;
	EmitVertex();

	data_out.vColor = data_in[1].vColor;
	data_out.texCoord = data_in[1].texCoord;
	data_out.fragPos = model * data_in[1].fragPos;
	data_out.normal = mat3(transpose(inverse(model))) * data_in[1].normal;
	data_out.tangent = mat3(transpose(inverse(model))) * data_in[1].tangent;
	gl_Position = projection * view * model * gl_in[1].gl_Position;
	EmitVertex();

	data_out.vColor = data_in[2].vColor;
	data_out.texCoord = data_in[2].texCoord;
	data_out.fragPos = model * data_in[2].fragPos;
	data_out.normal = mat3(transpose(inverse(model))) * data_in[2].normal;
	data_out.tangent = mat3(transpose(inverse(model))) * data_in[2].tangent;
	gl_Position = projection * view * model * gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();
}