#version 330 core

struct V2F
{
    vec3 positionWorld;
    vec3 normalWorld;
    vec4 color;
};

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;

out vec3 eyeDirection_worldspace;

out V2F  vdata;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 mesh_color;

void main()
{
    gl_Position = P * V * M * vec4(vertexPosition_modelspace, 1.f);

    vec3 vertexPosition_worldspace = (V * M * vec4(vertexPosition_modelspace, 1.f)).xyz;
    eyeDirection_worldspace = vec3(0.0f, 0.0f, 0.0f) - vertexPosition_worldspace;

    vdata.positionWorld = eyeDirection_worldspace;
    vdata.normalWorld   = mat3(transpose(inverse(M))) * vertexNormal_modelspace;
    vdata.color = vec4(mesh_color, 1.0f);
}
