//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//

#version 450 core

struct VsOut
{
    vec3 positionWorld;
    vec3 normalWorld;
    vec4 color;
    flat float specular_power;
};

out VsOut  vdata;

layout(location = 0) in vec3 aPosition; // vertex attribute
layout(location = 1) in vec3 aNormal;   // vertex attribute
layout(location = 2) in vec3 aInstanceOffset; // instance attribute 

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 u_mesh_color;

//< Lighting in Pixel shader, World Space.
void main()
{
    gl_Position = P * V * M * vec4(aPosition + aInstanceOffset, 1.f);

    uint i = gl_InstanceID / 10;
    uint j = gl_InstanceID % 10;
    float specular_power = 12.f * i + 1.f * j ;

    vec3 positionWorld = (M * vec4(aPosition + aInstanceOffset, 1.f)).xyz;

    vdata.positionWorld = positionWorld;
    vdata.normalWorld   = mat3(transpose(inverse(M))) * aNormal;
    vdata.color = vec4(u_mesh_color, 1.0f);
    vdata.specular_power = specular_power;
}
