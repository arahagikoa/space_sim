#version 330 core
out vec4 FragColor;

uniform vec3  uColor;
uniform float uAlpha;   // per-draw master opacity
in    float vAlpha;     // per-vertex fade

void main() {
    FragColor = vec4(uColor, uAlpha * vAlpha);
}