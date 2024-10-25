#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;

out DATA {
    vec4 vColor;
    vec2 texCoord;
    vec3 normal;
    vec4 fragPos;
} data_out;

uniform vec3 color;

void main() {
    gl_Position = vec4(aPos, 1.0);
    
    data_out.vColor = vec4(color, 1.f);
    data_out.texCoord = tex;
    data_out.fragPos = vec4(aPos, 1.0);
    data_out.normal = norm;

    //vColor = vec4(color, 1.f);
    //texCoord = tex;
    //fragPos = mat3(model) * aPos;
    //normal = norm;
}