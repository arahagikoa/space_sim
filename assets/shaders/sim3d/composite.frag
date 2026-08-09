#version 330 core
in  vec2 vNdc;
out vec4 FragColor;

uniform sampler2D uScene;   // linear radiance, possibly below window resolution
uniform sampler2D uBloom;
uniform float     uExposure;
uniform float     uBloomStrength;

vec3 aces(vec3 x) {
    const float a = 2.51, b = 0.03, c = 2.43, d = 0.59, e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
    vec2 uv = vNdc * 0.5 + 0.5;

    vec3 colour = texture(uScene, uv).rgb * uExposure
                + texture(uBloom, uv).rgb * uBloomStrength;

    colour = aces(colour);
    FragColor = vec4(pow(colour, vec3(1.0 / 2.2)), 1.0);
}
