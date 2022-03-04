//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//

#version 450 core

struct VsOut
{
    vec3 positionWorld;
    vec3 normalWorld;
    vec4 color;
    flat uint instaceId;
};

in VsOut vdata;  // same type, name with last stage

out vec4 fragColor;

uniform vec3 u_eye_position;

//< Lighting in Pixel shader, World Space.
void main()
{
    vec3 lightColor = vec3(1.f);
    vec3 L = normalize(vec3(1.f));

    uint i = vdata.instaceId / 10;
    uint j = vdata.instaceId % 10;
    float specular_power = 12 * i + 4 * j ;

    vec3 diffuse_albedo = vec3(0.5, 0.2, 0.7); //vdata.color.rgb;
    vec3 specular_albedo = vec3(0.7);

    vec3 N = normalize(vdata.normalWorld);
    if (!gl_FrontFacing) //< back face case
        N = -N;
    vec3 V = normalize(u_eye_position - vdata.positionWorld);
    vec3 R = reflect(-L, N);

    float diffuse  = max(dot(L, N), 0.0f);
    float specular = pow(max(dot(V, R), 0.0f), specular_power);

    if (!gl_FrontFacing) //< back face
    {
        diffuse_albedo = vec3(0.1f);
    }

    vec3 specularColor = specular * specular_albedo * lightColor ;
    vec3 diffuseColor  = diffuse * diffuse_albedo * lightColor ;
    vec3 ambientColor  = vec3(0.08f) * lightColor;

    vec3 finalColor = diffuseColor + ambientColor; 
    
    if (gl_FrontFacing) //< only front face has specular light
    {
        finalColor += specularColor; 
    }
    fragColor = vec4(finalColor, 1.0f);
}
