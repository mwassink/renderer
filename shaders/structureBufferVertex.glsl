#version 450 core

uniform mat4 mvp;
uniform mat4 mv;
layout (location = 0) in vec4 pos;
out vec4 posCameraSpace;
void main(void) {
    gl_Position = mvp * pos;
    posCameraSpace = vec4((mv * pos).xyz, 1.0);
}