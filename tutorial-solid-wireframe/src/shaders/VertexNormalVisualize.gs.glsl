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
out    vec4 color;

const float MAGNITUDE = 0.4f;

uniform mat4 V;
uniform mat4 P;

//< 
//< 
//< 
void LineFromVertex(int index)
{
    gl_Position = P * V * vec4(vdata[index].positionWorld, 1.f);
    color         = vdata[index].color;
    EmitVertex();
    gl_Position = P * V * vec4((vdata[index].positionWorld + vdata[index].normalWorld) * MAGNITUDE, 1.0f);
    color         = vdata[index].color;
    EmitVertex();
    EndPrimitive();
}


void main()
{
    LineFromVertex(0);
    LineFromVertex(1);
    LineFromVertex(2);
}

