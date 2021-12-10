#version 450 core

layout (binding = 0) uniform sampler2D img;
in vec2 uvpos;
out vec4 color;
void main() {
    color = texture(img, uvpos);
}