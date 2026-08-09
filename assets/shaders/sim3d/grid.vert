#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 uViewProj;

out float vRadius;

void main() {
    vRadius = length(aPos.xz);
    gl_Position = uViewProj * vec4(aPos, 1.0);
}
