#version 440 core

struct VertexOut
{
    vec3 positionWorld;
    vec3 normalWorld;
    vec4 color;
};

layout(triangles) in;
layout (line_strip, max_vertices = 6) out;  //< 3 lines

//< implicit array
in VertexOut vdata[];

uniform float u_normal_visualize_scale;

uniform mat4 V;
uniform mat4 P;

//< 
//< 
//< 
void LineFromVertex(int index)
{
    gl_Position = P * V * vec4(vdata[index].positionWorld, 1.f);
    EmitVertex();
    gl_Position = P * V * vec4((vdata[index].positionWorld + normalize(vdata[index].normalWorld)) * u_normal_visualize_scale, 1.0f);
    EmitVertex();
    EndPrimitive();
}


void main()
{
    LineFromVertex(0);
    LineFromVertex(1);
    LineFromVertex(2);
}

