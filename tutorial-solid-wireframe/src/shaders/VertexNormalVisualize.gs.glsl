#version 440 core

struct VertexOut
{
    vec3 normalView;
};

layout(triangles) in;
layout (line_strip, max_vertices = 6) out;  //< 3 lines

//< implicit array
in VertexOut vdata[];

uniform float u_normal_visualize_scale;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

//< 
//< 
//< 
void LineFromVertex(int index)
{
    gl_Position = P * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = P * ( gl_in[index].gl_Position +
                        vec4(vdata[index].normalView, 0.f) * u_normal_visualize_scale
                       );
    EmitVertex();
    EndPrimitive();
}


void main()
{
    LineFromVertex(0);
    LineFromVertex(1);
    LineFromVertex(2);
}

