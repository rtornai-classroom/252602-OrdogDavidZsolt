#version 330 core
layout (location = 0) in vec3 vPos;
uniform mat4 matProjection;
uniform mat4 matModelView;
void main() {
    gl_Position = matProjection * matModelView * vec4(vPos, 1.0);
}
