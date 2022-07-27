#version 450

uniform int seed;
uniform float zoom;
uniform vec2 location;
uniform vec2 resolution;

float PHI = 1.61803398874989484820459;  // Golden Ratio   

float gold_noise(vec2 xy, int seed){
       return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

void main()
{  
    float value = gold_noise(gl_FragCoord.xy, seed);
    gl_FragColor = vec4(vec3(value), 1.0);
}