#version 440 core

struct V2G
{
    vec3 position;
    vec3 normal;
    vec4 color;
};

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;

out V2G vdata;

uniform vec3 mesh_color;

void main()
{
    vdata.position = vertexPosition_modelspace.xyz;
    vdata.normal = normalize(vertexNormal_modelspace);
    vdata.color  = vec4(mesh_color, 1.0f);
}
