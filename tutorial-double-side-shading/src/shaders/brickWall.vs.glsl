#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aTexCoords;

out VS_OUT 
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform vec3 u_light_pos;
uniform vec3 u_viewer_pos;

void main()
{
    vs_out.FragPos = vec3(M * vec4(aPos, 1.0));   
    vs_out.TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(M)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));    
    vs_out.TangentLightPos = TBN * u_light_pos;
    vs_out.TangentViewPos  = TBN * u_viewer_pos;
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;
    vs_out.Normal = N;
        
    gl_Position = P * V * M * vec4(aPos, 1.0);
    gl_ClipDistance[0] = dot(vec4(M * vec4(aPos, 1.0)), vec4(0,0,-1, -0.3));
}