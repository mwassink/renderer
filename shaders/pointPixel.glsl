#version 450 core

#define M_PI 3.1415926535897932384626433832795

layout (binding = 0) uniform sampler2D tex;
layout (binding = 1) uniform sampler2D normalMap;
layout (binding = 2) uniform samplerCubeShadow depthMap;



uniform vec3 specularColor;
uniform vec3 lightColor;
uniform float lightBrightness;
uniform vec2 FN;
    
in vec2 uvCoord;
in vec3 lightDir;
in vec3 eyeDir;
in vec4 shadowCoord;

in float distSquared;
in float zTest;

const float shininess = 16.0f;
const float ambientCoeff = .05f;

out vec4 color;

void colorFace(vec3 pt) {
    vec3 pIn = vec3(abs(pt.x), abs(pt.y), abs(pt.z));
    if (pIn.x > pIn.y) {
        if (pIn.x > pIn.z) {
            if (pt.x > 0) {
                color = vec4(1.0f, 0, 1.0f, 1.0f);
            } else {
                color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
            
        } else { 
            color = vec4(0, 0, 1.0f, 1.0f);
        }
    } else {
        if (pIn.y > pIn.z) {
            color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
        } else { 
            color = vec4(0, 0, 1.0f, 1.0f);
        }
    }
        
}


float getDepth(vec4 pIn) {
    float zSample = max(abs(pIn.x), max(abs(pIn.y), abs(pIn.z)));
    
    float normZ = (FN.x + FN.y)/(FN.x - FN.y) - (2*FN.x * FN.y)/(FN.x-FN.y)/zSample;
    return  (normZ + 1.0) * 0.5;
}

    
float fetchCoeff(vec4 posIn) {
    
      
      vec4 posNew = vec4(posIn.x, posIn.y, -posIn.z, getDepth(posIn));
      
    float s = texture(depthMap, posNew); // do not need to divide by w, just use direction vector to sample the cube map

    return s;
    
}


void main(void) {

    vec3 l = normalize(lightDir);
    vec3 v = normalize(eyeDir);
    vec4 normalRaw = texture(normalMap, uvCoord);
    vec3 n = normalize(2.0*normalRaw.xyz - 1.0); // [0, 1] -> [-1, 1]
    vec3 diffColor = texture(tex, uvCoord).xyz;

    float lambertian = max(dot(n, l), 0.0f);
    vec3 h = normalize(n + v);
    float spec = pow(max(dot(n, h), 0.0f), shininess);
    
    float scaleQuad = 1.0f / (4*M_PI*distSquared);
    float lightIntensity = lightBrightness * scaleQuad;

    float s = fetchCoeff(shadowCoord);
    vec3 diffRefl = (lambertian* lightIntensity*s)*diffColor*lightColor;

    vec3 specRefl = s * spec * lightIntensity * lightColor * specularColor;
    vec3 ambient = ambientCoeff * diffColor;
    color = vec4(diffRefl + specRefl + ambient, 1.0f);
    //color = vec4(diffColor, 1);

    
    
}

