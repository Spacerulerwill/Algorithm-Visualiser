#version 450

uniform int seed;
uniform float zoom;
uniform vec2 location;
uniform vec2 resolution;

float PHI = 1.61803398874989484820459;  // Golden Ratio   
float PI = 3.1415926535;

float gold_noise(vec2 xy, int seed){
       return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

float perlin_noise(vec2 coord) {
    // which square we are in
    vec2 i = floor(coord);
    vec2 f = fract(coord);

    // angles for each corner
    float tl = gold_noise(i, seed) * 2 * PI;
    float tr = gold_noise(i + vec2(1.0, 0.0), seed) * 2 * PI;
    float bl = gold_noise(i + vec2(0.0, 1.0), seed) * 2 * PI;
    float br = gold_noise(i + vec2(1.0, 1.0), seed) * 2 * PI;

    // rotate vector of length 0, 1 by corresponding angle
    vec2 tlvec = vec2(-sin(tl), cos(tl));
    vec2 trvec = vec2(-sin(tr), cos(tr));
    vec2 blvec = vec2(-sin(bl), cos(bl));
    vec2 brvec = vec2(-sin(br), cos(br));

    //dot product between distance of rotated vector and the distance vector of pixel to its corner
    float tldot = dot(tlvec, f);
    float trdot = dot(trvec, f - vec2(1.0, 0.0));
    float bldot = dot(blvec, f - vec2(0.0, 1.0));
    float brdot = dot(brvec, f - vec2(1.0,1.0));

    vec2 cubic = f * f * (3.0 - 2.0 * f);

    float topmix = mix(tldot, trdot, cubic.x);
    float botmix = mix(bldot, brdot, cubic.x);
    float wholemix = mix(topmix, botmix, cubic.y);

    return wholemix + 0.5;
}

void main()
{
    vec2 uv = gl_FragCoord.xy / resolution;
    uv.x *= resolution.x / resolution.y;
    uv -= vec2((resolution.x/resolution.y)/2, 0.5); //move to center
    uv *= zoom;
    uv += location;

    float value = perlin_noise(uv);
    gl_FragColor = vec4(vec3(value), 1.0);
}