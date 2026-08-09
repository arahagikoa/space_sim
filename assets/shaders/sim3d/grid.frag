#version 330 core
in  float vRadius;
out vec4 FragColor;

uniform vec3  uColor;
uniform float uFadeStart;   // world radius where the mesh starts dissolving
uniform float uFadeEnd;     // and where it is gone

void main() {
    float fade = 1.0 - smoothstep(uFadeStart, uFadeEnd, vRadius);
    if (fade <= 0.0) discard;

    FragColor = vec4(uColor, fade);
}
