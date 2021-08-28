#version 450 core

uniform mat4 modelViewProjection;
layout (location = 0) in vec4 pos;
void main(void) {
    gl_Position = modelViewProjection * pos;
}