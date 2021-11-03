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

//< would it be possible to organize it as an struct array
out    noperspective vec3 height;
out    vec3 positionWorld;
out    vec3 normalWorld;
out    vec4 color;

const float MAGNITUDE = 0.4;

//< 
//< 
//< 
void main()
{
    gl_Position =  gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = (gl_in[0].gl_Position + vec4(gs_in[0].normal, 0.0)) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
}

