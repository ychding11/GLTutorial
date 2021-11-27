#version 440 core

in noperspective vec3 height;
in vec3 positionWorld;
in vec3 normalWorld;
in vec4 color;

out vec4 fragColor;

uniform vec3 u_eye_position;
uniform int u_show_wireframe;

//< Lighting in Pixel shader, World Space.
void main()
{
    //float minHeight = min(height);
    float minHeight = min(min(height[0],height[1]),height[2]);
    float edgeIntensity = exp2(-1.0*minHeight*minHeight);
    if (u_show_wireframe == 0)
        edgeIntensity  = 0.f;

    vec3 lightColor = vec3(1.f);
    vec3 diffuseColor = color.rgb;

    vec3 n = normalize(normalWorld);
    vec3 l = normalize(vec3(1.f));
    float diffuse = clamp(dot(n, l), 0.f, 1.f);

    vec3 v = normalize(u_eye_position - positionWorld);
    vec3 r = reflect(-l, n);
    float specular = pow(max(dot(v, r), 0.0), 32);

    vec3 specularLight = 0.1f * specular * lightColor * diffuseColor;
    vec3 diffuseLight  = 0.84f * diffuse * lightColor * diffuseColor;
    vec3 ambientLight  = vec3(0.06f) * lightColor;

    //colorFrag = specularLight + diffuseLight + ambientLight; 
    vec3 finalColor = specularLight + diffuseLight + ambientLight; 
    fragColor = (edgeIntensity * vec4(0.1,0.1,0.1,1.0)) + ((1.0-edgeIntensity) * vec4(finalColor,1.f));
}
