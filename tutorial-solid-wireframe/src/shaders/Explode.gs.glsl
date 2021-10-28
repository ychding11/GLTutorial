#version 440 core

struct V2G
{
    vec3 position;
    vec3 normal;
    vec4 color;
};

/*
 * Each individual variable will be an array of the primitive's vertex count; 
 * in this shader : 3
*/
in V2G vdata[];

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

//uniform float time;

// GLSL Hacker automatic uniform
//uniform mat4 gxl3d_ModelViewProjectionMatrix;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

uniform float explode_scale;
uniform float explode_frequency;
uniform float frame_id;

out vec4 vertexColor;
out float strength;

/*
 * https://www.khronos.org/opengl/wiki/Geometry_Shader
 Geometry Shaders provide the following built-in input variables:
 These variables have only the meaning the prior shader stage(s) that passed them gave them.
    in gl_PerVertex
    {
      vec4 gl_Position;
      float gl_PointSize;
      float gl_ClipDistance[];
    } gl_in[];

    gl_PrimitiveIDIn : the current input primitive's ID, based on the number of primitives processed by the GS since the current drawing command started.
    gl_InvocationID  : the current instance, as defined when instancing geometry shaders.

   Geometry Shaders have the following built-in outputs. These outputs are always associated with stream 0.
   - if you're emitting vertices to a different stream, you don't have to write to them.
   - you must write to some of these values if you emit to stream 0(default stream) and enable rasterizer

    out gl_PerVertex
    {
      vec4 gl_Position; //the clip-space output position of the current vertex. This value must be written if you are emitting a vertex to stream 0, unless rasterization is off.
      float gl_PointSize;
      float gl_ClipDistance[];
    };


*/
void main()
{
    vec3 P0 = vdata[0].position.xyz;
    vec3 P1 = vdata[1].position.xyz;
    vec3 P2 = vdata[2].position.xyz;

    vec3 E0 = P0 - P1;
    vec3 E1 = P2 - P1;

    vec3 N = normalize(cross(E1, E0)); //< surface normal
    float diff_len = length(E1 - E0);

    //if (diff_len > 0.001f)
    {
        for (int i = 0; i < gl_in.length(); i++)
        {
            //float verb = explode_scale * ( 1.f + cos(2.f * 3.1415926f * explode_frequency * frame_id) );
            //float verb = explode_scale * ( cos(frame_id) );
            //< built-in variable gl_Position must be written, to enable rasterizer
            gl_Position  = vec4(vdata[i].position + (N * explode_scale), 1.0);
            gl_Position = P * V * M * gl_Position;

            vertexColor = vdata[i].color;
            strength = explode_scale;
            EmitVertex();
        }
        EndPrimitive();
    }
}
