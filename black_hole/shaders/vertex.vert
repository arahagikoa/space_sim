#version 330 core
layout (location = 0) in vec2 aPos;
layout(location = 1) in float aAlpha;

out float vAlpha;

void main() {
    float x = aPos.x;
    float y = aPos.y;
    gl_Position = vec4(aPos, 0.0, 1.0);

    gl_PointSize = 1.0;
    vAlpha = aAlpha;
}