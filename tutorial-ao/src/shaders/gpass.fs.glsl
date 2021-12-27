#version 440 core

struct VertexOut
{
    vec3 positionWorld;
    vec3 normalWorld;
    vec4 color;
};
in VertexOut  vdata;

out vec4 fragColor;

layout (location = 0) out vec3 gbufferPosition;
layout (location = 1) out vec3 gbufferNormal;
layout (location = 2) out vec3 gbufferAlbedo;

//< Lighting in Pixel shader, World Space.
void main()
{
    gbufferPosition = vdata.positionWorld;
    gbufferNormal = normalize(vdata.normalWorld);
    gbufferAlbedo = vdata.color.rgb;
}
