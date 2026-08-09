#version 330 core
in  vec2 vNdc;
out vec4 RayDir;

uniform vec3 cameraPos;
uniform mat3 basis;
uniform float aspect_ratio;
uniform float uFovY;

void main() {
    float t = vNdc.x * 0.5 + 0.5;       // 0 on the left, 1 on the right

    float h = tan(radians(uFovY) * 0.5);
    vec2  p = vec2(vNdc.x * aspect_ratio * h, vNdc.y * h);
    vec3 dir = normalize(basis * vec3(p, 1.0));
    RayDir = vec4(dir * 0.5 + 0.5, 1.0);
}