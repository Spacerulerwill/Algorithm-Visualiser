#pragma once
#include "ScreenElement.h"

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
    int mousePosLoc;
    int juliaModeLoc;

    float color_1 = 0.0f;
    float color_2 = 0.0f;
    float color_3 = 0.0f;
    float color_4 = 0.0f;
    int iterations = 100;
    float resolution[2] = {getWidth(), getHeight()};

    Vector2 mousePos = { 0,0 };
    bool juliaMode = false;
    float location[2] = {0, 0};
    float zoom  = 2.0f;

    int selectedFractal = 0;
    const char* fractals[3] = { "Mandelbrot Set", "Burning Ship Fractal", "Tricorn"};

    Shader mandelbrot;
    Shader burningShip;
    Shader tricorn;
    Shader* activeFractal = &mandelbrot;

    bool fractalSelector;

    RenderTexture2D target;

    void setFractal(Shader& shader);
   
};

