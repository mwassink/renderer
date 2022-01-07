#version 450 core

// (TODO) this should use a cone
uniform mat4 modelViewProjection;
uniform mat4 modelView;
uniform mat4 shadowMatrix;
uniform mat3 normalMatrix;
uniform vec3 lightCameraSpace;
uniform vec2 FNTest;
uniform mat4 modelLightMatrix;


layout (location = 0) in vec4 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec2 uvVertex;
layout (location = 4) in float handedness;

out vec2 uvCoord;
out vec3 lightDir;
out vec3 eyeDir;
out vec4 shadowCoord;
out float distSquared;
out vec4 posModel;
out float zTest;

void getDepth(vec4 pIn) {
    float zSample = max(abs(pIn.x), max(abs(pIn.y), abs(pIn.z)));
    
    float normZ = (FNTest.x + FNTest.y)/(FNTest.x - FNTest.y) - (2*FNTest.x * FNTest.y)/(FNTest.x-FNTest.y)/zSample;
    zTest = (normZ + 1.0) * 0.5;
    
}

void main(void) {
    vec3 n = normalize(normalMatrix * normal);
    vec3 t = normalize(normalMatrix * tangent);
    vec3 b = cross(n, t);
    b *= handedness;
    
    vec3 eyePos = (modelView * pos).xyz; // pos of pt in camera space. confusing...
    vec3 diff = lightCameraSpace - eyePos; // from pos to light
    lightDir = vec3(dot(diff, t), dot(diff, b), dot(diff, n) );
    lightDir = normalize(lightDir);

    eyeDir = vec3(dot(-eyePos, t), dot(-eyePos, b), dot(-eyePos, n));
    eyeDir = normalize(eyeDir);

    gl_Position = modelViewProjection * pos;
    distSquared = dot(diff, diff);
    shadowCoord = modelLightMatrix * pos;
    uvCoord = uvVertex;
    posModel = pos;
    getDepth(shadowCoord);
}