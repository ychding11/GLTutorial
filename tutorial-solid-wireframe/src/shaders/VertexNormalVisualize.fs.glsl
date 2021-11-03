#version 440 core

in  vec4 color;

out vec4 fragColor;

uniform vec3 u_eye_position;

//< Lighting in Pixel shader, World Space.
void main()
{
    fragColor = color;
}
