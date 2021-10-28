#version 440 core

in  vec4 vertexColor;
in  float strength;

out vec4 FragColor;

void main()
{
    //FragColor = vertexColor * (strength + 0.1);
    FragColor = vertexColor;
}
