#version 440 core

in vec3 gVoxelPos;

layout(binding = 1, offset = 0) uniform atomic_uint uCounter;

layout(std430, binding = 2) 
writeonly buffer uuFragmentList
{
    uvec2 uFragmentList[];
};

uniform int u_count_voxel_only;


//< Lighting in Pixel shader, World Space.
void main()
{
     uint curIndex = atomicCounterIncrement(uCounter);
     if (u_count_voxel_only != 0)
     {
        //It cause crash in intel gpu
     	uvec3 uvoxel_pos = clamp(uvec3(gVoxelPos), uvec3(0u), uvec3(1024 - 1u));
		uFragmentList[curIndex].x = uvoxel_pos.x | (uvoxel_pos.y << 12u) | ((uvoxel_pos.z & 0xffu) << 24u); //only have the last 8 bits of uvoxel_pos.z
		uFragmentList[curIndex].y = ((uvoxel_pos.z >> 8u) << 28u) | 0 | (0 << 8u) | (10 << 16u);
     }
}
