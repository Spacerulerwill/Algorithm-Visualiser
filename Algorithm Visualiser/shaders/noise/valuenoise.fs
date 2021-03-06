#version 450

uniform int seed;
uniform float zoom;
uniform vec2 location;
uniform vec2 resolution;
uniform int octaves;

float PHI = 1.61803398874989484820459;  // Golden Ratio   

float gold_noise(vec2 xy, int seed){
       return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

float value_noise(vec2 coord){
    vec2 i = floor(coord);
    vec2 f = fract(coord);

    float tl = gold_noise(i, seed);
    float tr = gold_noise(i + vec2(1.0, 0.0), seed);
    float bl = gold_noise(i + vec2(0.0, 1.0), seed);
    float br = gold_noise(i + vec2(1.0, 1.0), seed);

    vec2 cubic = f * f * (3.0 - 2.0 * f);

    float topmix = mix(tl, tr, cubic.x);
    float botmix = mix(bl, br, cubic.x);
    float wholemix = mix(topmix, botmix, cubic.y);

    return wholemix;
}

float fbm(vec2 coord){       
    float normalised_factor = 0.0;
    float value = 0.0;
    float scale = 0.5;

    for (int i = 0; i < octaves; i++){
        value += value_noise(coord) * scale;
        coord *= 2;
        normalised_factor += scale;
        scale *= 0.5;
    }
    return value / normalised_factor;
}

void main()
{
    vec2 uv = gl_FragCoord.xy / resolution;
    uv.x *= resolution.x / resolution.y;
    uv -= vec2((resolution.x/resolution.y)/2, 0.5); //move to center
    uv *= zoom;
    uv += location;

    float value = fbm(uv);
    gl_FragColor = vec4(vec3(value), 1.0);
    
}