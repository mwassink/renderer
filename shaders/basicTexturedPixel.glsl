#version 450 core

layout (binding = 0) uniform sampler2D tex;
layout (binding = 1) uniform sampler2D normalMap;

uniform vec3 specularColor;
uniform vec3 lightColor;
uniform float lightBrightness;

in vec2 uvCoord;
in vec3 lightDir;
in vec3 eyeDir;
in float distSquared;

const float shininess = 16.0f;
const float ambientCoeff = .05f;

out vec4 color;

void main(void) {
    lightDir = normalize(lightDir);
    eyeDir = normalize(eyeDir);
    vec4 normalOld = texture(normalMap, uvCoord);
    vec3 n = normalize(2.0*normalOld.xyz - 1.0);
    vec4 diffColor = texture(tex, uvCoord);
    
    float lambertian = max(dot(n, lightDir ), 0.0f);
    vec3 h = normalize(n + eyeDir);
    float spec = pow(max(dot(n, h), 0.0f), shininess);
    
    float scaleQuad = 1.0f / (4*M_PI*distSquared);
    float lightIntensity = lightBrightness * scaleQuad;
    
    vec3 diffRefl = (lambertian/M_PI* lightIntensity + lightIntensity*ambientCoeff)*diffColor*lightColor;

    vec3 specRefl = spec * lightIntensity * lightColor * specularColor;
    color = vec4(diffRefl + specRefl, 1.0f);
    
}
