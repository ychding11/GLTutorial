#version 440 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;


out    vec3 gVoxelPos;

vec2 Project(in vec3 v, in int axis)
{
    return axis == 0 ? v.yz : (axis == 1 ? v.xz : v.xy);
}

//< 
//< http://strattonbrazil.blogspot.com/2011/09/single-pass-wireframe-rendering_10.html 
//< 
void main()
{
    vec3 axis_weight = abs(cross(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz, gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz));
	int picked_axis = 2;
	if(axis_weight.x >= axis_weight.y && axis_weight.x > axis_weight.z)
        picked_axis = 0;
	else if(axis_weight.y >= axis_weight.z && axis_weight.y > axis_weight.x)
        picked_axis = 1;

    gVoxelPos = (gl_in[0].gl_Position.xyz + 1.0f) * 0.5f * 1024; //< adjust to [0, 1]
	gl_Position = vec4(Project(gl_in[0].gl_Position.xyz, picked_axis), 1.0f, 1.0f);
    EmitVertex();

    gVoxelPos = (gl_in[1].gl_Position.xyz + 1.0f) * 0.5f * 1024; //< adjust to [0, 1]
	gl_Position = vec4(Project(gl_in[1].gl_Position.xyz, picked_axis), 1.0f, 1.0f);
    EmitVertex();

    gVoxelPos = (gl_in[2].gl_Position.xyz + 1.0f) * 0.5f * 1024; //< adjust to [0, 1]
	gl_Position = vec4(Project(gl_in[2].gl_Position.xyz, picked_axis), 1.0f, 1.0f);
    EmitVertex();
}

