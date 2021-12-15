#version 440 core

out vec4 FragColor;
  
in vec2 texCoord;

uniform int u_pp_filter;

uniform sampler2D u_tex_color_map;

const vec2 texelsize = vec2(1.0 / 1280.0, 1.0/720.0);  //hardcode framebuffer size

const vec2 offsets[9] = vec2[]
    (
        vec2(-1.0f,  1.0f),     // top-left
        vec2( 0.0f,    1.0f),   // top-center
        vec2( 1.0f,  1.0f),     // top-right
        vec2(-1.0f,  0.0f),     // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( 1.0f,  0.0f),     // center-right
        vec2(-1.0f, -1.0f),     // bottom-left
        vec2( 0.0f,   -1.0f),   // bottom-center
        vec2( 1.0f, -1.0f)      // bottom-right    
    );

//< sharpen kernel
vec3 sharpen()
{
    float kernel[9] = float[]
    (
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(u_tex_color_map, texCoord.st + offsets[i]*texelsize));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    return col;
}

//< blur kernel
vec3 blur()
{
    float kernel[9] = float[]
    (
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
    );
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(u_tex_color_map, texCoord.st + offsets[i]*texelsize));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    return col;
}

void main()
{
    if (u_pp_filter == 0) // None
        FragColor = texture(u_tex_color_map, texCoord);
    else if (u_pp_filter == 1) // Sharpen
        FragColor = vec4(sharpen(), 1.0);
    else if (u_pp_filter == 2) // Blur
        FragColor = vec4(blur(), 1.0);
    else 
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
