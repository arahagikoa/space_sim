#version 330 core
in  vec2 vNdc;
out vec4 FragColor;

uniform mat3      basis;         // columns: right, up, forward - all unit length
uniform vec3      cameraPos;     // ray origin, world space
uniform mat4      uViewProj;     // matches basis/cameraPos; used only for depth
uniform float     aspect_ratio;
uniform float     uFovY;         // vertical field of view, in degrees
uniform sampler2D uSky;          // equirectangular background

uniform float uRs;               // Schwarzschild radius, world units
uniform float uDiskInner;        // accretion disk radii, world units
uniform float uDiskOuter;

uniform int   uSteps;            // integrator budget per ray
uniform float uTime;             // seconds, drives the disk's rotation
uniform bool  uDoppler;          // apply beaming and redshift to the disk
uniform bool  uShowShadowRing;   // draw the theoretical shadow edge over the render

const float PI = 3.14159265359;

const float STEP_FRACTION = 0.05;
const float SKY_BRIGHTNESS = 0.25;

const int RAY_ESCAPED = 0;
const int RAY_DISK    = 1;
const int RAY_HORIZON = 2;

// --- Accretion disk ---------------------------------------------------------
const float DISK_T_PEAK  = 11000.0;

const float PROFILE_PEAK = 0.4879;

const float ORBIT_RATE  = 6.0;   // visual speed of the Keplerian shear
const float NOISE_SCALE = 2.5;   // turbulence cells per Schwarzschild radius

vec2 dir_to_equirect(vec3 dir) {
    float lon = atan(dir.z, dir.x);              // -PI .. PI
    float lat = acos(clamp(dir.y, -1.0, 1.0));   // 0 at +Y pole .. PI at -Y pole

    return vec2(lon / (2.0 * PI) + 0.5,
                1.0 - lat / PI);
}

vec3 sample_sky(vec3 dir) {
    vec2 uv = dir_to_equirect(dir);

    vec2 dx = dFdx(uv);
    vec2 dy = dFdy(uv);
    dx.x = fract(dx.x + 0.5) - 0.5;
    dy.x = fract(dy.x + 0.5) - 0.5;

    return textureGrad(uSky, uv, dx, dy).rgb * SKY_BRIGHTNESS;
}

vec3 gravity(vec3 pos, float h2) {
    float r2 = dot(pos, pos);
    return -1.5 * uRs * h2 * pos / (r2 * r2 * sqrt(r2));   // r^5
}

float march_step(vec3 pos) {
    return STEP_FRACTION * length(pos);
}

int trace(vec3 pos, inout vec3 vel, out vec3 hit, out float r) {
    vec3  L  = cross(pos, vel);
    float h2 = dot(L, L);

    vec3 acc = gravity(pos, h2);

    for (int i = 0; i < uSteps; ++i) {
        float dt   = march_step(pos);
        vec3  next = pos + vel * dt + 0.5 * acc * dt * dt;

        if (pos.y * next.y < 0.0) {
            vec3  crossing = mix(pos, next, pos.y / (pos.y - next.y));
            float radius   = length(crossing.xz);

            if (radius > uDiskInner && radius < uDiskOuter) {
                hit = crossing;
                r = radius;
                return RAY_DISK;
            }
        }

        if (dot(next, next) < uRs * uRs) {
            hit = next;
            return RAY_HORIZON;
        }

        vec3 nextAcc = gravity(next, h2);
        vel += 0.5 * (acc + nextAcc) * dt;

        pos = next;
        acc = nextAcc;
    }

     hit = pos;
    return dot(pos, pos) < 9.0 * uRs * uRs ? RAY_HORIZON : RAY_ESCAPED;
}

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

// Value noise: hash the four lattice corners and smoothstep between them.
float value_noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 w = f * f * (3.0 - 2.0 * f);

    return mix(mix(hash(i),                  hash(i + vec2(1.0, 0.0)), w.x),
               mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), w.x), w.y);
}

float fbm(vec2 p) {
    float sum = 0.0, amp = 1.0, norm = 0.0;

    for (int i = 0; i < 3; ++i) {
        sum  += amp * value_noise(p);
        norm += amp;
        p    *= 2.0;
        amp  *= 0.5;
    }
    return sum / norm;
}

float planck(float lambda, float T) {
    return 1.0 / (pow(lambda, 5.0) * (exp(14388.0 / (lambda * T)) - 1.0));
}

vec3 blackbody(float T) {
    T = max(T, 800.0);

    vec3 c = vec3(planck(0.60, T), planck(0.55, T), planck(0.45, T));
    return c / max(max(c.r, c.g), c.b);
}

float disk_temperature(float r) {
    float x = r / uDiskInner;                       // inversesqrt(x) = sqrt(r_in/r)
    float profile = pow(x, -0.75) * pow(max(1.0 - inversesqrt(x), 0.0), 0.25);

    return DISK_T_PEAK * profile / PROFILE_PEAK;
}

vec3 orbit_dir(vec3 p) {
    return normalize(cross(vec3(0.0, 1.0, 0.0), p));
}

vec3 disk_colour(vec3 hit, float r, vec3 vel) {
    float T = disk_temperature(r);

    if (uDoppler) {
        float beta  = sqrt(0.5 * uRs / (r - uRs));
        float gamma = inversesqrt(1.0 - beta * beta);

        vec3  toCamera = -normalize(vel);
        float doppler  = 1.0 / (gamma * (1.0 - beta * dot(orbit_dir(hit), toCamera)));

        T *= doppler * sqrt(1.0 - uRs / r);
    }

    float phi = uTime * ORBIT_RATE * pow(r / uRs, -1.5);
    float c = cos(phi), s = sin(phi);
    float turb = fbm(mat2(c, s, -s, c) * hit.xz * (NOISE_SCALE / uRs));

    float glow = pow(T / DISK_T_PEAK, 4.0);

    return blackbody(T) * glow * mix(0.55, 1.35, turb);
}

float depth_of(vec3 world) {
    vec4 clip = uViewProj * vec4(world, 1.0);
    if (clip.w <= 0.0) return 1.0;

    return clamp(0.5 * (clip.z / clip.w) + 0.5, 0.0, 1.0);
}

void main() {
    float tanHalfFov = tan(radians(uFovY) * 0.5);
    vec2  p   = vec2(vNdc.x * aspect_ratio, vNdc.y) * tanHalfFov;
    vec3  vel = normalize(basis * vec3(p, 1.0));

    float b = length(cross(cameraPos, vel));

    vec3  hit;
    float r;
    int   outcome = trace(cameraPos, vel, hit, r);

    vec3 colour = sample_sky(normalize(vel));

    if (outcome == RAY_DISK) {
        colour = disk_colour(hit, r, vel);
    } else if (outcome == RAY_HORIZON) {
        colour = vec3(0.0);
    }

    if (uShowShadowRing) {
        float bCrit = 1.5 * sqrt(3.0) * uRs;
        float edge  = 1.0 - smoothstep(0.0, 1.5 * fwidth(b), abs(b - bCrit));
        colour = mix(colour, vec3(0.0, 1.0, 0.8), edge);
    }

    FragColor    = vec4(colour, 1.0);
    gl_FragDepth = outcome == RAY_ESCAPED ? 1.0 : depth_of(hit);
}
