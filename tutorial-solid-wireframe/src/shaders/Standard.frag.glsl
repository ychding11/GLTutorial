#version 330 core

struct V2F
{
    vec3 positionWorld;
    vec3 normalWorld;
    vec4 color;
};

in V2F vdata;

out vec3 colorFrag;

uniform vec3 u_eye_position;

//< Lighting in Pixel shader, World Space.
void main()
{
    vec3 lightColor = vec3(1.f);
    float lightPower = 1.0f;

    vec3 materialDiffuseColor = vdata.color.rgb;

    vec3 n = normalize(vdata.normalWorld);
    vec3 l = normalize(vec3(1.f));
    float diffuse = clamp(dot(n, l), 0.f, 1.f);

    vec3 v = normalize(u_eye_position - vdata.positionWorld);
    vec3 r = reflect(-l, n);
    float specular = pow(max(dot(v, r), 0.0), 32);

    vec3 specularLight = 0.1f * specular * lightColor * materialDiffuseColor;
    vec3 diffuseLight  = 0.84f * diffuse * lightColor * materialDiffuseColor;
    vec3 ambientLight  = vec3(0.06f) * lightColor;

    colorFrag = specularLight + diffuseLight + ambientLight; 
}
