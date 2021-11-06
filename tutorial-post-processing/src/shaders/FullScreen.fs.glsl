#version 440 core

out vec4 FragColor;
  
in vec2 texCoord;

uniform sampler2D u_tex_color_map;

void main()
{ 
    FragColor = texture(u_tex_color_map, texCoord);
}
