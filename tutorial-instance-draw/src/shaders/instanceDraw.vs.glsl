#version 330 core

struct V2F
{
    vec3 positionWorld;
    vec3 normalWorld;
    vec4 color;
};

layout(location = 0) in vec3 aPosition; // vertex attribute
layout(location = 1) in vec3 aNormal;   // vertex attribute
layout(location = 2) in vec3 aInstanceOffset; // instance attribute 

out V2F  vdata;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 u_mesh_color;

//< Lighting in Pixel shader, World Space.
void main()
{
    gl_Position = P * V * M * vec4(aPosition + aInstanceOffset, 1.f);

    vec3 positionWorld = (M * vec4(aPosition + aInstanceOffset, 1.f)).xyz;

    vdata.positionWorld = positionWorld;
    vdata.normalWorld   = mat3(transpose(inverse(M))) * aNormal;
    vdata.color = vec4(u_mesh_color, 1.0f);
}
