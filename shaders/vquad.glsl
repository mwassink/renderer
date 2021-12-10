#version 450 core

layout (location = 0) in vec2 pos;
out vec2 uvpos;
void main() {
    uvpos = 0.5 * pos.xy + vec2(0.5); // (0,0), (2,0), (0,2)
    gl_Position = vec4(pos.xy, 0, 1); // (-1, -1), (3,-1), (-1, 3)
}