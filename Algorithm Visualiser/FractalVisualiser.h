#pragma once
#include "ScreenElement.h"
#include <complex>
#include <cmath>

class FractalVisualiser :

    public ScreenElement
{
public:

    FractalVisualiser(float xRatio, float yRatio, float widthRatio, float heightRatio);

    //override draw method
    void draw() override;
    void keyEvents() override;
private:

    int color_1Loc;   
    int color_2Loc;
    int color_3Loc;
    int color_4Loc;
    int iterationsLoc;
    int resolutionLoc;
    int zoomLoc;
    int locationLoc;

    float color_1;
    float color_2;
    float color_3;
    float color_4;
    int iterations = 50;
    float resolution[2] = {getWidth(), getHeight()};
    float location[2] = {0, 0};
    float zoom  = 1.0f;

    int selectedFractal = 0;
    const char* fractals[4] = { "Mandelbrot Set", "Julia Set", "Burning Ship Fractal", "Tricorn"};

    Shader mandelbrot;
    Shader burningShip;
    Shader tricorn;
    Shader julia;
    Shader* activeFractal = &mandelbrot;

    bool fractalSelector;

    RenderTexture2D target;

    void setFractal(Shader& shader);
   
};

