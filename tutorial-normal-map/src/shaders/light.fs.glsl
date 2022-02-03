//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//

#version 330 core

out vec4 FragColor;

in VS_OUT 
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;


uniform vec3 u_light_color;

void main()
{       
    vec3 normal = normalize(fs_in.Normal);

    // diffuse
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 diffuse  = max(dot(viewDir, normal), 0.0) * u_light_color;

    FragColor = vec4(0.2 * diffuse + 0.8 * u_light_color, 1.0);
}