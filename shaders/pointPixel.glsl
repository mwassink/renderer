#version 450 core

#define M_PI 3.1415926535897932384626433832795

layout (binding = 0) uniform sampler2D tex;
layout (binding = 1) uniform sampler2D normalMap;
layout (binding = 2) uniform samplerCubeShadow depthMap;



uniform vec3 specularColor;
uniform vec3 lightColor;
uniform float lightBrightness;
// ASSERT these are neg when they get plumbed into the fn
// The algo requires this
uniform vec2 AB; // A B of the proj matrix (2,2), (2,3)
    
in vec2 uvCoordl;
in vec3 lightDir;
in vec3 eyeDir;
// this is the  coord in the space of the shadow
in vec4 shadowCoord; //  the same one that we used when rendering, so this should be ok

in float distSquared;

const float shininess = 16.0f;
const float ambientCoeff = .05f;

float fetchCoeff(vec3 posIn) {
    // the depth will be the maximum value (e.g if x is largest, then we will use +x and then the depth )
    posIn = abs(posIn);
    float maxDepth = max(posIn.x, max(posIn.y, posIn.z));
    // With the OpenGL matrix given, (more negative) z values are mapped into larger z values after proj
    // If I were to simply use the coeffs A and B, then the depth would be negative as A and B are neg
    // The thing that saves it with the projection is that when the z value is stored as w it gets
    // multiplied by -1 
    float depth = -AB.x - AB.y / maxDepth; //those are surely neg values so the comp would always succeed otherwise
    
}


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

    float s = fetchCoeff(depthTexture, shadowCoord );
    //float s = 1;
    vec3 diffRefl = (lambertian* lightIntensity*s)*diffColor*lightColor;

    vec3 specRefl = s * spec * lightIntensity * lightColor * specularColor;
    vec3 ambient = ambientCoeff * diffColor;


    color = vec4(diffRefl + specRefl + ambient, 1.0f);
}

