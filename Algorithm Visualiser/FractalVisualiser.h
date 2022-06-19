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

    ~FractalVisualiser();

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
    float location[2] = {0.0f, 0.0f };

    Vector2 mousePos = {0,0};
    bool juliaMode = false;
    bool juliaFrozen = false;
    float zoom = 2.0f;

    int selectedFractal = 0;
    const char* fractals[3] = { "Mandelbrot", "Burning Ship", "Tricorn"};

    // preset color schemes
    int selectedColorPreset = 0;
    const char* colorPresetNames[4] = {"Default", "Electric Blue", "Blood Red", "Neon Green"};
    const float colorPresets[4][4] = { {0.0, 0.0, 0.0, 0.0}, {0.109, 0.134, 0.144, 0.791}, {0.199, 0.159, 0.164, 1.0}, {1.0, 0.308, 0.194, 1.0} };

    Shader mandelbrot;
    Shader burningShip;
    Shader tricorn;
    Shader* activeFractal = &mandelbrot;

    bool fractalSelector;
    bool colorSelector;

    Texture2D texture;

    void setFractal(Shader& shader);
    float mapToReal(float x, float minR, float maxR);
    float mapToImaginary(float x, float minI, float maxI);

};

