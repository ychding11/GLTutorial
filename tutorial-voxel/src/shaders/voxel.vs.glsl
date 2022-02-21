#version 440 core

layout(location = 0) in vec3 aPosition; //vertex attribute
layout(location = 1) in vec3 aNormal;  //vertex attribute


//uniform vec3 u_mesh_color;

//< Lighting in Pixel shader, World Space.
void main()
{
    //gl_Position = P * V * M * vec4(aPosition, 1.f);
    gl_Position = vec4(aPosition, 1.f);
}
