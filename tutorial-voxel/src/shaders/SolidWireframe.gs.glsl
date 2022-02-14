#version 440 core

struct VertexOut
{
    vec3 positionWorld;
    vec3 normalWorld;
    vec4 color;
};

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

//< implicit array
in VertexOut vdata[];

//< would it be possible to organize it as an struct array
out    noperspective vec3 height;
out    vec3 positionWorld;
out    vec3 normalWorld;
out    vec4 color;

uniform vec2 u_window_size;

vec2 Project(in vec3 v, in int axis)
{
    return axis == 0 ? v.yz : (axis == 1 ? v.xz : v.xy);
}

//< 
//< http://strattonbrazil.blogspot.com/2011/09/single-pass-wireframe-rendering_10.html 
//< 
void main()
{
    //< calcuate window space position
    vec2 p0 = u_window_size * gl_in[0].gl_Position.xy/gl_in[0].gl_Position.w;
    vec2 p1 = u_window_size * gl_in[1].gl_Position.xy/gl_in[1].gl_Position.w;
    vec2 p2 = u_window_size * gl_in[2].gl_Position.xy/gl_in[2].gl_Position.w;
    vec2 e0 = p2-p1;
    vec2 e1 = p2-p0;
    vec2 e2 = p1-p0;
    float area = abs(e1.x*e2.y - e1.y*e2.x); //< cross product

    vec3 axis_weight = abs(cross(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz, gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz));

	int picked_axis = 2;
	if(axis_weight.x >= axis_weight.y && axis_weight.x > axis_weight.z)
        picked_axis = 0;
	else if(axis_weight.y >= axis_weight.z && axis_weight.y > axis_weight.x)
        picked_axis = 1;

    height        = vec3(area/length(e0),0,0);
    positionWorld = vdata[0].positionWorld;
    normalWorld   = vdata[0].normalWorld;
    color         = vdata[0].color;
    gl_Position   =  gl_in[0].gl_Position;
    EmitVertex();

    height        = vec3(0,area/length(e1),0);
    positionWorld = vdata[1].positionWorld;
    normalWorld   = vdata[1].normalWorld;
    color         = vdata[1].color;
    gl_Position   =  gl_in[1].gl_Position;
    EmitVertex();

    height        = vec3(0,0,area/length(e2));
    positionWorld = vdata[2].positionWorld;
    normalWorld   = vdata[2].normalWorld;
    color         = vdata[2].color;
    gl_Position   =  gl_in[2].gl_Position;
    EmitVertex();
}

