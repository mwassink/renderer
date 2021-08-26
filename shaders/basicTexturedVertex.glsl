#version 450 core

uniform mat4 modelViewProjection;
uniform mat4 modelView;
uniform mat3 normalMatrix;

layout (location = 0) in vec4 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec2 uvCoord;


layout (binding = 0) uniform sampler2D texture;
layout (binding = 1) uniform sampler2D normalMap;

out vec2 uvCoord;
out vec3 vertCameraSpace;

void main(void) {
    gl_Position = modelViewProjection * pos;
}