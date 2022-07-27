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

float cellular_noise(vec2 coord) {
    // which square we are in
    vec2 i = floor(coord);
    vec2 f = fract(coord);

    float min_dist = 99999.0;

    for (float x = -1.0; x <= 1.0; x++){
        for (float y = -1.0; y <= 1.0; y++){
            vec2 node = vec2(gold_noise(vec2(i.x+x, i.y+y), seed), gold_noise(vec2(i.x+x, i.y+y), seed+1)) + vec2(x,y);
            
            float dist = sqrt((f - node).x * (f - node).x + (f - node).y * (f - node).y);
			min_dist = min(min_dist, dist);
        }
    }
    return min_dist;
}

void main()
{
    vec2 uv = gl_FragCoord.xy / resolution;
    uv.x *= resolution.x / resolution.y;
    uv -= vec2((resolution.x/resolution.y)/2, 0.5); //move to center
    uv *= zoom;
    uv += location;

    float value = cellular_noise(uv);
    gl_FragColor = vec4(vec3(value), 1.0);
}