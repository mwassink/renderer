#version 450 core

uniform mat4 modelViewProjection;
uniform mat4 modelView;
uniform mat3 normMatrix;
uniform vec3 lightCameraSpace;

layout (location = 0) in vec4 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec2 uvVertex;
layout (location = 4) in float handedness;

out vec2 uvCoord;
out vec3 lightDir;
out vec3 eyeDir;
out float distSquared;

void main(void) {
    vec3 n = normalize(normMatrix * normal);
    vec3 t = normalize(normMatrix * tangent);
    vec3 b = cross(n, t);
    b *= handedness;
    
    vec3 eyePos = (modelView * pos).xyz;
    vec3 diff = lightCameraSpace - eyePos;
    lightDir = vec3(dot(diff, t), dot(diff, b), dot(diff, n) );
    lightDir = normalize(lightDir);

    eyeDir = vec3(dot(-eyePos, t), dot(-eyePos, b), dot(-eyePos, n));
    eyeDir = normalize(eyeDir);

    gl_Position = modelViewProjection * pos;
    distSquared = length(diff);
    uvCoord = uvVertex;
}