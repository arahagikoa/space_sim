#version 330 core
in  vec2 vNdc;
out vec4 FragColor;

uniform sampler2D uSource;
uniform vec2      uDirection;   // one texel along the axis being blurred

const float OFFSETS[3] = float[](0.0, 1.3846153846, 3.2307692308);
const float WEIGHTS[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

void main() {
    vec2 uv = vNdc * 0.5 + 0.5;

    vec3 sum = texture(uSource, uv).rgb * WEIGHTS[0];

    for (int i = 1; i < 3; ++i) {
        vec2 step = uDirection * OFFSETS[i];
        sum += texture(uSource, uv + step).rgb * WEIGHTS[i];
        sum += texture(uSource, uv - step).rgb * WEIGHTS[i];
    }

    FragColor = vec4(sum, 1.0);
}
