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

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

//uniform vec3 lightPos;
//uniform vec3 viewPos;
uniform vec3 u_light_color;
uniform int  use_normal_map;

void main()
{       
    vec3 normal;
    if (use_normal_map == 1)
    {
        // obtain normal from normal map in range [0,1]
        // in tangent space, then convert it into [-1, 1]
        normal = texture(normalMap, fs_in.TexCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
    }
    else
    {
        normal = normalize(fs_in.Normal);
    }

    if (!gl_FrontFacing) normal = -normal;

    vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb;

    // ambient
    vec3 ambient = 0.08 * color;

    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    vec3 diffuse  = max(dot(lightDir, normal), 0.0) * color * u_light_color;

    if (!gl_FrontFacing)
        diffuse  = max(dot(lightDir, normal), 0.1) * vec3(0.2f) * u_light_color;

    // specular
    vec3 viewDir    = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec * u_light_color;

    if (gl_FrontFacing)
    {
        FragColor = vec4(ambient + diffuse + specular, 1.0);
    }
    else
    {
        FragColor = vec4(ambient + diffuse, 1.0);
    }
}