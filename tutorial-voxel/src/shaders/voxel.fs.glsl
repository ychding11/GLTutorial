#version 440 core

in noperspective vec3 height;
in vec3 positionWorld;
in vec3 normalWorld;
in vec4 color;
in vec3 gVoxelPos;

layout(binding = 1, offset = 0) uniform atomic_uint uCounter;
layout(std430, binding = 2)     writeonly buffer uuFragmentList { uvec2 uFragmentList[]; };

out vec4 fragColor;

uniform vec3 u_eye_position;
uniform int u_show_wireframe;
uniform int u_count_voxel_only;


//< Lighting in Pixel shader, World Space.
void main()
{
     uint curIndex = atomicCounterIncrement(uCounter);
     if (u_count_voxel_only != 0)
     {
     	uvec3 uvoxel_pos = clamp(uvec3(gVoxelPos), uvec3(0u), uvec3(1024 - 1u));
		uFragmentList[curIndex].x = uvoxel_pos.x | (uvoxel_pos.y << 12u) | ((uvoxel_pos.z & 0xffu) << 24u); //only have the last 8 bits of uvoxel_pos.z
		uFragmentList[curIndex].y = ((uvoxel_pos.z >> 8u) << 28u) | 0 | (0 << 8u) | (10 << 16u);
     }

    //float minHeight = min(height);
    float minHeight = min(min(height[0],height[1]),height[2]);
    float edgeIntensity = exp2(-1.0*minHeight*minHeight);
    if (u_show_wireframe == 0)
        edgeIntensity  = 0.f;

    vec3 lightColor = vec3(1.f);
    vec3 diffuseColor = color.rgb;

    vec3 n = normalize(normalWorld);
    vec3 l = normalize(vec3(1.f));
    float diffuse = clamp(dot(n, l), 0.f, 1.f);

    vec3 v = normalize(u_eye_position - positionWorld);
    vec3 r = reflect(-l, n);
    float specular = pow(max(dot(v, r), 0.0), 32);

    vec3 specularLight = 0.1f * specular * lightColor * diffuseColor;
    vec3 diffuseLight  = 0.84f * diffuse * lightColor * diffuseColor;
    vec3 ambientLight  = vec3(0.06f) * lightColor;

    vec3 finalColor = specularLight + diffuseLight + ambientLight; 
    //fragColor = (edgeIntensity * vec4(0.1,0.1,0.1,1.0)) + ((1.0-edgeIntensity) * vec4(finalColor,1.f));
    fragColor = vec4(gVoxelPos, 1.0); 
}
