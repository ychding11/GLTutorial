#version 440 core

struct VertexOut
{
    vec3 positionWorld;
    vec3 normalWorld;
    vec4 color;
};

layout(location = 0) in vec3 aPosition; //vertex attribute
layout(location = 1) in vec3 aNormal;  //vertex attribute

out VertexOut  vdata;

uniform mat4 M;
uniform vec3 u_mesh_color;

//< Lighting in Pixel shader, World Space.
void main()
{
    gl_Position =  M * vec4(aPosition, 1.f);

    vdata.positionWorld = (M * vec4(aPosition, 1.f)).xyz;;
    vdata.normalWorld   = mat3(transpose(inverse(M))) * aNormal;
    vdata.color = vec4(u_mesh_color, 1.0f);
}
