#version 330 core

struct V2F
{
    vec3 positionWorld;
    vec3 normalWorld;
    vec4 color;
};

in vec3 eyeDirection_worldspace;
in V2F vdata;

out vec3 colorFrag;

uniform vec3 lightPosition_worldspace;

void main()
{
    vec3 lightColor = vec3(1, 1, 0.5);
    float lightPower = 600.0f;

    vec3 materialDiffuseColor = vdata.color.rgb;
    vec3 materialAmbientColor = vec3(0.3, 0.3, 0.3) * materialDiffuseColor;
    vec3 materialSpecularColor = vec3(0.5, 0.5, 0.5) * materialDiffuseColor;

    float distance = length(lightPosition_worldspace - vdata.positionWorld);

    vec3 n = normalize(vdata.normalWorld);
    vec3 l = normalize(lightPosition_worldspace - vdata.positionWorld);

    float cosTheta = clamp(dot(n, l), 0.f, 1.f);

    vec3 e = normalize(eyeDirection_worldspace);
    vec3 r = reflect(-l, n);

    float cosAlpha = clamp(dot(e,r), 0.f, 1.f);

    colorFrag = materialAmbientColor
	    + materialDiffuseColor * lightColor * lightPower * cosTheta / (distance * distance)
	    + materialSpecularColor * lightColor * lightPower * pow(cosAlpha, 5) / (distance * distance);
}
