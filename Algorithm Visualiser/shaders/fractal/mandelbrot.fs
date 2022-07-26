#version 450

uniform vec2 resolution;
uniform vec2 location;
uniform vec2 mousePos;
uniform int juliaMode;
uniform float zoom;
uniform float color_1;
uniform float color_2;
uniform float color_3;
uniform float color_4;
uniform int iterations;

vec2 compsquare(vec2 z)
{
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

vec3 mandelbrot(vec2 point){
    vec2 z;

    if (juliaMode == 1){ //z is point - julia set
      z = point;
      point = mousePos;

    }else{ //z starts at 0 and is squared, with point added on - mandelbrot set
        z = vec2(0.0);
    }

    //calculate iterationts until it escapes
    float iters = 0.0;
    for (iters = 0.0; iters < float(iterations); ++iters)
    {
        z = compsquare(z) + point;
        if(dot(z, z) > 4.0) break;
    }

    //calculate color based on that
    float hue = iters / float(iterations);

    return hsv2rgb(vec3(hue, 1.0, 1.0));
}

void main()
{
    vec2 uv = gl_FragCoord.xy / resolution;

    uv.x *= resolution.x / resolution.y;
    uv -= vec2((resolution.x/resolution.y)/2, 0.5); //move center of mandelbrot to center
    
    if (juliaMode == 1){
        uv *= 2.0;
    }else{
        uv *= zoom; //zoom
        uv += location;
    }
    
    uv.y *= -1;

    gl_FragColor = vec4(mandelbrot(uv), 1.0);
}