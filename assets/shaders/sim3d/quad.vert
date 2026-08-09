#version 330 core
layout(location = 0) in vec2 aPos;
out vec2 vNdc;

void main() {
    vNdc = aPos;                        // -1..1 across the screen
    gl_Position = vec4(aPos, 0.0, 1.0);
}