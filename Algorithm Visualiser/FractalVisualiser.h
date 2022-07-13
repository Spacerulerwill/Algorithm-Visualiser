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

    //uniform locations locations
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

    //uniforms
    float color_1 = 0.0f;
    float color_2 = 0.0f;
    float color_3 = 0.0f;
    float color_4 = 0.0f;
    int iterations = 100;
    float resolution[2] = {getWidth(), getHeight()};
    float location[2] = {0.0f, 0.0f };
    float zoom = 2.0f;
    bool fractalSelector;
    bool colorSelector;
    Vector2 mousePos = {0,0};
    bool juliaMode = false;
    bool stabilityVisualiser = false;

    // fractal selector
    int selectedFractal = 0;
    const static int numFractals = 3;
    const char* fractals[numFractals] = { "Mandelbrot", "Burning Ship", "Tricorn"};

    // preset color schemes
    int selectedColorPreset = 0;
    const static int numColors = 5;

    const char* colorPresetNames[numColors] = {"Default", "Electric Blue", "Blood Red", "Neon Green", "Pretty Pink"};
    const float colorPresets[numColors][4] = { 
        {0.0f, 0.0f, 0.0f, 0.0f}, 
        {0.109f, 0.134f, 0.144f, 0.791f},
        {0.199f, 0.159f, 0.164f, 1.0f}, 
        {1.0f, 0.308f, 0.194f, 1.0f},
        {0.213f, 0.131f,0.197f, 1.0f}
    };

    //fractal shaders
    Shader mandelbrot;
    Shader burningShip;
    Shader tricorn;
    Shader* activeFractal;

    Texture2D realTimeTexture; //real time texture

    //high precision fractal settings
    Image highPrecisionImage;
    Texture2D highPrecisionTexture;
    int percentLoaded = 0;
    bool shouldLoadHighPrecisionTexture = false;
    bool isLoadingHighPrecisionFractal = false;

    // fractal functions
    void setFractal(Shader& shader);
    float mapToReal(float x, float minR, float maxR);
    float mapToImaginary(float x, float minI, float maxI);

    void renderRealTimeFractal();
    void drawCalculatedFractalToImage();

    void drawFractalLoadPercentage();

    Vector2 mandelbrotFormula(Vector2 z, Vector2 c);
    Vector2 burningshipFormula(Vector2 z, Vector2 c);
    Vector2 tricornFormula(Vector2 z, Vector2 c);

    bool highPrecisionMode = false;
    bool juliaFrozen = false;

};

