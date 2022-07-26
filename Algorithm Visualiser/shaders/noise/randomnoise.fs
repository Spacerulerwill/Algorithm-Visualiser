#version 450

float random2d(vec2 coord){
  return fract(sin(dot(coord.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    gl_FragColor = vec4(0.5, 0.5,0.5, 1.0);
}