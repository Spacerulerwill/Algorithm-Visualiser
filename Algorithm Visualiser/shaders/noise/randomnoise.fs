#version 450

uniform int time;

float PHI = 1.61803398874989484820459;  // Golden Ratio   

float gold_noise(vec2 xy, int seed){
       return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

void main()
{

    float red = gold_noise(gl_FragCoord.xy, time);
    float green = gold_noise(gl_FragCoord.xy, time+1);
    float blue = gold_noise(gl_FragCoord.xy, time+2);
    gl_FragColor = vec4(red,green,blue, 1.0);
}