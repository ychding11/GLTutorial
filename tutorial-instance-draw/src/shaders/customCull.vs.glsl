#version 330 core

struct V2F
{
    vec3 positionWorld;
    vec3 normalWorld;
    vec4 color;
};

layout(location = 0) in vec3 aPosition; //vertex attribute
layout(location = 1) in vec3 aNormal;  //vertex attribute

out V2F  vdata;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 u_mesh_color;

uniform float u_clip_plane_dist;

//< Lighting in Pixel shader, World Space.
void main()
{
    gl_Position = P * V * M * vec4(aPosition, 1.f);
    gl_ClipDistance[0] = dot(vec4(M * vec4(aPosition, 1.0)), vec4(0,0,-1, -u_clip_plane_dist));

    vec3 positionWorld = (V * M * vec4(aPosition, 1.f)).xyz;

    vdata.positionWorld = positionWorld;
    vdata.normalWorld   = mat3(transpose(inverse(M))) * aNormal;
    vdata.color = vec4(u_mesh_color, 1.0f);
}
