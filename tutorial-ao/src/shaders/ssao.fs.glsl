
#version 440 core

out float FragColor;

in vec2 texCoord;

uniform sampler2D texPosition;
uniform sampler2D texNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64]; //< in view space

uniform mat4 P;

int sampleNum = 64;
float radius = 0.5;
float bias = 0.025;

// tile noise texture over screen based on screen dimensions divided by noise size
const vec2 noiseScale = vec2(1280/4.0, 720.0/4.0); 


void main()
{
    vec3 fragPos = texture(texPosition, texCoord).xyz;
    vec3 normal  = normalize(texture(texNormal, texCoord).rgb);
    vec3 randomVec = normalize(texture(texNoise, texCoord * noiseScale).xyz);

    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    //<
    //< OpenGL uses column-major matrices
    //<
    mat3 TBN = mat3(tangent, bitangent, normal);

    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < sampleNum; ++i)
    {
        vec3 samplePos = TBN * samples[i]; // tangent to view-space
        samplePos = fragPos + samplePos * radius; 
        
        //< convert to NDC space
        vec4 offset = vec4(samplePos, 1.0);
        offset = P * offset;    // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = 0.5 * (offset.xyz  + 1.f); // transform to range [0.0 , 1.0]
        
        float sampledZ = texture(texPosition, offset.xy).z;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampledZ));
        occlusion += (sampledZ >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0f - (occlusion / sampleNum);
    
    FragColor = occlusion;
}