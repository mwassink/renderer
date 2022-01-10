#version 450 core
in vec4 posCameraSpace;
void main(void) {
    color = posCameraSpace;
}