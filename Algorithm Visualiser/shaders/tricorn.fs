#version 450

uniform vec2 resolution;
uniform vec2 location;
uniform vec2 mousePos;
uniform bool juliaMode;
uniform float zoom;
uniform float color_1;
uniform float color_2;
uniform float color_3;
uniform float color_4;
uniform int iterations;

vec2 conjsquare(vec2 z)
{
    z = vec2(z.x, -z.y);
    float temp = z.x;
    z.x = z.x * z.x - z.y * z.y;
    z.y = 2.0 * temp * z.y;
    return z;
}

vec3 hsv2rgb(vec3 hue){
    vec4 K = vec4(color_1, color_2, color_3, color_4);
    vec3 p = abs(fract(hue.xxx + K.xyz) * 6.0 - K.www);
    return hue.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), hue.y);
}

float mapToReal(float x, float minR, float maxR){
    float range = maxR-minR;
    return x * (range / resolution.x) + minR;
}

float mapToImaginary(float y, float minI, float maxI){
    float range = maxI - minI;
    return y * (range / resolution.y) + minI;
}

vec3 tricorn(vec2 point){
    vec2 z;
    if (juliaMode){ //z is point - julia set
        z = point;
        point = mousePos;
    }else{ //z starts at 0 and is squared, with point added on - mandelbrot set
        z = vec2(0.0);
    }

    float iters = 0.0;
    for (iters = 0.0;   iters < float(iterations); ++iters)
    {
        z = conjsquare(z) + point;
        if(dot(z, z) > 4.0) break;
    }
    float hue = iters / float(iterations);
    return hsv2rgb(vec3(hue, 1.0, 1.0));
}

void main()
{
    vec2 uv = gl_FragCoord.xy / resolution;
    uv.x *= resolution.x / resolution.y;
    uv -= vec2((resolution.x/resolution.y)/2, 0.5);

    if (juliaMode){
        uv *= 2.5;
    }
    else{
        uv *= zoom; //zoom
        uv -= location;
    }
    gl_FragColor = vec4(tricorn(uv), 1.0);
}