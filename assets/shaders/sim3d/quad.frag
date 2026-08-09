#version 330 core
in  vec2 vNdc;
out vec4 FragColor;

uniform mat3      basis;         // columns: right, up, forward - all unit length
uniform vec3      cameraPos;     // ray origin, world space
uniform float     aspect_ratio;
uniform float     uFovY;         // vertical field of view, in degrees
uniform sampler2D uSky;          // equirectangular background

uniform float uDiskInner;        // accretion disk radii, in Schwarzschild radii
uniform float uDiskOuter;

const float PI = 3.14159265359;

// Unit direction -> equirectangular texture coordinates.
// +Y is the pole axis, matching the camera's world_up.
vec2 dir_to_equirect(vec3 dir) {
    float lon = atan(dir.z, dir.x);              // -PI .. PI
    float lat = acos(clamp(dir.y, -1.0, 1.0));   // 0 at +Y pole .. PI at -Y pole

    // clamp() above matters: float error can push dir.y just past 1.0,
    // and acos() of that is NaN.
    return vec2(lon / (2.0 * PI) + 0.5,
                1.0 - lat / PI);
}

vec3 sample_sky(vec3 dir) {
    vec2 uv = dir_to_equirect(dir);

    // uv.x jumps 1->0 where longitude wraps. The hardware picks a mip level from
    // screen-space derivatives, and that jump reads as an enormous rate of change,
    // so one pixel column would sample the blurriest mip - a hard vertical seam.
    // Wrapping the derivative back into [-0.5, 0.5] recovers the true value.
    vec2 dx = dFdx(uv);
    vec2 dy = dFdy(uv);
    dx.x = fract(dx.x + 0.5) - 0.5;
    dy.x = fract(dy.x + 0.5) - 0.5;

    return textureGrad(uSky, uv, dx, dy).rgb;
}

// Analytic intersection of ray(t) = ro + t*rd with the plane y = 0.
// Straight lines only - no gravity yet.
bool hit_disk(vec3 ro, vec3 rd, out vec3 hit, out float r) {
    // Parallel to the disk: no intersection, and dividing by rd.y would blow up.
    if (abs(rd.y) < 1e-6) return false;

    float t = -ro.y / rd.y;
    if (t <= 0.0) return false;      // plane lies behind the camera

    hit = ro + t * rd;
    r = length(hit.xz);
    return r > uDiskInner && r < uDiskOuter;
}

// Checkerboard in (radius, angle). Deliberately plain - once rays bend, the
// distortion of these straight cell edges is what makes the lensing readable.
vec3 disk_colour(vec3 hit, float r) {
    float rad = (r - uDiskInner) / (uDiskOuter - uDiskInner);   // 0..1 outward
    float ang = atan(hit.z, hit.x) / (2.0 * PI) + 0.5;          // 0..1 around

    float checker = mod(floor(rad * 8.0) + floor(ang * 24.0), 2.0);

    vec3 warm = mix(vec3(1.00, 0.78, 0.42), vec3(0.72, 0.26, 0.10), rad);
    return warm * mix(0.45, 1.0, checker);
}

void main() {
    float h = tan(radians(uFovY) * 0.5);
    vec2  p = vec2(vNdc.x * aspect_ratio * h, vNdc.y * h);
    vec3  dir = normalize(basis * vec3(p, 1.0));

    vec3 colour = sample_sky(dir);

    vec3  hit;
    float r;
    if (hit_disk(cameraPos, dir, hit, r)) {
        colour = disk_colour(hit, r);
    }

    FragColor = vec4(colour, 1.0);
}
