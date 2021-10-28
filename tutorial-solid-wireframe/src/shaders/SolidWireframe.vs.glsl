#version 330 core

struct V2F
{
    vec3 positionWorld;
    vec3 normalWorld;
    vec4 color;
};

layout(location = 0) in vec3 aPosition; //vertex attribute
layout(location = 1) in vec3 aNormal;  //vertex attribute

//out vec3 eyeDirection_worldspace;
out V2F  vdata;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 u_mesh_color;

//< Lighting in Pixel shader, World Space.
void main()
{
    gl_Position = P * V * M * vec4(aPosition, 1.f);

    vec3 positionWorld = (V * M * vec4(aPosition, 1.f)).xyz;
    //eyeDirection_worldspace = vec3(0.0f, 0.0f, 0.0f) - positionWorld;

    vdata.positionWorld = positionWorld;
    vdata.normalWorld   = mat3(transpose(inverse(M))) * aNormal;
    vdata.color = vec4(u_mesh_color, 1.0f);
}
