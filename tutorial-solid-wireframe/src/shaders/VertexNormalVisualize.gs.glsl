#version 440 core

layout(triangles) in;
layout (line_strip, max_vertices = 6) out;  //< 3 lines

// implicit array as input for geometry shader
// same varaible name, type for link between shader stage
in  vec3 normalView[];

uniform float u_normal_visualize_scale;

uniform mat4 P;

//< 
//< 
void LineFromVertex(int index)
{
    gl_Position = P * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = P * ( gl_in[index].gl_Position +
                        vec4(normalView[index], 0.f) * u_normal_visualize_scale
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

