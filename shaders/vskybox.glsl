#version 450 core

layout (location = 0) in vec3 inPos;

out vec3 tCoord;

// the camera has a different basis than the world
// we do not need mObject. It would just be some identity matrix
uniform mat4 viewProjection; 

void main(void) {
    tCoord = inPos;
    gl_Position = (viewProjection * vec4(inPos, 1.0f)).xyww;
}

