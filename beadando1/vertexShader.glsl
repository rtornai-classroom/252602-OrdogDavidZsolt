#version 330

layout (location = 0) in vec2 aPosition;
uniform vec2 u_center;
out vec2 varyingPosition;

void main(void) {
    varyingPosition = aPosition + u_center;
    gl_Position = vec4(varyingPosition, 0.0, 1.0);
}