#version 440 core

struct TessEvalOut
{
    vec3 position;
    vec3 normal;
    vec4 color;
};

layout(triangles) in;
//layout(triangle_strip, max_vertices = 3) out;
layout(points, max_vertices = 3) out;

/*
 * Each individual variable is an array of the primitive's vertex. 
 * Array size if vertex count, in this case : 3
*/
in TessEvalOut tedata[];

uniform float u_explode_scale;
//uniform float frame_id;
uniform vec3 u_model_center;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

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
    vec3 P0 = tedata[0].position.xyz;
    vec3 P1 = tedata[1].position.xyz;
    vec3 P2 = tedata[2].position.xyz;

    vec3 E0 = P0 - P1;
    vec3 E1 = P2 - P1;

    vec3 N = normalize(cross(E1, E0)); //< surface normal
    {
        for (int i = 0; i < gl_in.length(); i++)
        {
            vec3 p = tedata[i].position;
            vec3 n = tedata[i].normal;
            float factor = dot(N, n);
            vec3 dir = normalize(p - u_model_center);
            dir = normalize(dir + n);
            vec3 det = dir * factor * u_explode_scale;
            
            //< built-in variable gl_Position must be written, to enable rasterizer
            gl_Position = P * V * M * vec4(p + det, 1.0);
            //gl_PointSize = 10;

            vertexColor = tedata[i].color;
            strength    = length(det);
            EmitVertex();
        }
        EndPrimitive();
    }
}
