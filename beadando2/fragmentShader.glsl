#version 330 core
out vec4 fragColor;
uniform vec3 objectColor;
uniform int isPoint;
void main() {
    if (isPoint == 1) {
        float r = 0.0;
        vec2 c = gl_PointCoord - vec2(0.5);
        r = dot(c, c);
        if (r > 0.25) discard;
    }
    fragColor = vec4(objectColor, 1.0);
}
