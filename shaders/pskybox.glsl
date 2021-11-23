#version 450 core

in vec3 tCoord;

layout (location = 0) out vec4 color;
uniform samplerCube tex;

void main() {
    color = texture(tex, tCoord);
}