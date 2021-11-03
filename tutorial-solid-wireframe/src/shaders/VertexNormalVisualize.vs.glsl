#version 440 core

layout(location = 0) in vec3 aPosition; //vertex attribute
layout(location = 1) in vec3 aNormal;   //vertex attribute

out vec3 normalView;

uniform mat4 M;
uniform mat4 V;

//< 
//< normal is affected by scale and rotation
//< transform it into view space
//< 
void main()
{
    gl_Position = V * M * vec4(aPosition, 1.f);
    mat3 normalMatrix = mat3(transpose(inverse(V * M)));
    normalView   = normalize(vec3(vec4(normalMatrix * aNormal, 0.0)));;
}
