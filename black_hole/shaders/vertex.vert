#version 330 core
layout (location = 0) in vec2 aPos;

void main() {
    float x = aPos.x / (800.0 / 2.0) - 1.0;
    float y = aPos.y / (600.0 / 2.0) - 1.0;
    gl_Position = vec4(aPos, 0.0, 1.0);

    gl_PointSize = 1.0;
}