#version 330

in vec2 varyingPosition;

uniform vec2 u_center;
uniform int u_objectType;   // 0 = kör, 1 = kék vonal
uniform int u_swapColors;   // 0 = Nincs metszés (piros közép, zöld szél), 1 = Metszés (zöld közép, piros szél)

out vec4 outColor;

void main(void) {
    if (u_objectType == 1) {
        outColor = vec4(0.0, 0.0, 1.0, 1.0);
        return;
    }

    float radius = 0.1666667; 
    float dist = distance(varyingPosition, u_center);
    if (dist > radius) {
        discard;
    }

    float ratio = dist / radius;

    vec4 redColor = vec4(1.0, 0.0, 0.0, 1.0);
    vec4 greenColor = vec4(0.0, 1.0, 0.0, 1.0);
    vec4 centerColor = (u_swapColors == 1) ? greenColor : redColor;
    vec4 edgeColor   = (u_swapColors == 1) ? redColor : greenColor;

    // ratio=0.0 -> 100% centerColor
    // ratio=1.0 -> 100% edgeColor
    // ratio=0.5 -> Fele-fele arányú
    outColor = mix(centerColor, edgeColor, ratio);
}