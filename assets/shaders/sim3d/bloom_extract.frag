#version 330 core
in  vec2 vNdc;
out vec4 FragColor;

uniform sampler2D uScene;
uniform float     uExposure;
uniform float     uThreshold;   // in display-referred units, post-exposure

void main() {
    vec3 c = texture(uScene, vNdc * 0.5 + 0.5).rgb * uExposure;

    float lum = dot(c, vec3(0.2126, 0.7152, 0.0722));

    FragColor = vec4(c * max(lum - uThreshold, 0.0) / max(lum, 1e-4), 1.0);
}
