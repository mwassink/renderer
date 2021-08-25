#version 450 core

#define M_PI 3.1415926535897932384626433832795

const float shininess =  4.0f;
uniform vec3 lightCameraSpace;
uniform vec3 diffColor;
uniform vec3 specularColor;
uniform vec3 lightColor;
uniform float lightBrightness;

const float ambientCoeff = .10f;
in vec3 normCameraSpace;
in vec3 vertCameraSpace;


out vec4 color;

void main(void) {
    vec3 l = normalize(lightCameraSpace - vertCameraSpace);
    vec3 n = normalize(normCameraSpace);
    //vec3 n = (1.0f, 0, 0);
    vec3 v = normalize(-vertCameraSpace);
    float lambertian = max(dot(n, l), 0.0);
    
    vec3 h = normalize(n + v);
    float spec = pow(max(dot(n, h), 0.0f), shininess);

    // Attenuate light power as a sphere
    float scaleQuad  = 1.0f / (4*M_PI*dot(l,l));
    float lightIntensity = lightBrightness * scaleQuad;
    
    vec3 diffRefl = (lambertian/M_PI* lightIntensity + lightIntensity* ambientCoeff)*diffColor*lightColor;
    vec3 specRefl = spec * lightIntensity * lightColor * specularColor;
    color = vec4(diffRefl + specRefl, 1.0f);

    
}