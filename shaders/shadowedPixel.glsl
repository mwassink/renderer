#version 450 core

#define M_PI 3.1415926535897932384626433832795


layout (binding = 0) uniform sampler2D tex;
layout (binding = 1) uniform sampler2D normalMap;
layout (binding = 2) uniform sampler2DShadow depthTexture;

uniform vec3 specularColor;
uniform vec3 lightColor;
uniform float lightBrightness;

in vec2 uvCoord;
in vec3 lightDir;
in vec3 eyeDir;
in vec4 shadowCoord;
in float distSquared;

const float shininess = 16.0f;
const float ambientCoeff = .05f;

out vec4 color;

void main(void) {
    vec3 l = normalize(lightDir);
    vec3 v = normalize(eyeDir);
    vec4 normalOld = texture(normalMap, uvCoord);
    vec3 n = normalize(2.0*normalOld.xyz - 1.0);
    vec3 diffColor = texture(tex, uvCoord).xyz;

    
    float lambertian = max(dot(n, l), 0.0f);
    vec3 h = normalize(n + v);
    float spec = pow(max(dot(n, h), 0.0f), shininess);
    
    float scaleQuad = 1.0f / (4*M_PI*distSquared);
    float lightIntensity = lightBrightness * scaleQuad;

    float s = texture(depthTexture, shadowCoord );
    vec3 diffRefl = (lambertian/M_PI* lightIntensity*s)*diffColor*lightColor;

    vec3 specRefl = s * spec * lightIntensity * lightColor * specularColor;
    vec3 ambient = ambientCoeff * diffColor;


    color = vec4(diffRefl + specRefl + ambient, 1.0f);
    
}