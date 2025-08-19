#version 330 core

out vec4 FragColor;
uniform vec3 uColor;

uniform float uAlpha; 
in float vAlpha;

void main() {
    float alpha = max(uAlpha, vAlpha);  // use whichever is set
    FragColor = vec4(uColor, alpha);
}