#pragma once
#include "ScreenElement.h"
#include <string>

class FractalVisualiser :

    public ScreenElement
{
public:

    FractalVisualiser(float xRatio, float yRatio, float widthRatio, float heightRatio);

    ~FractalVisualiser();

private:
    //override draw method
    void draw() override;
    void keyEvents() override;
    void events() override;

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
    Vector2 location = {0.0f, 0.0f };
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

    //buttons and stuff
    bool realInputBox;
    bool imaginaryInputBox;
    bool zoomInputBox;
    bool renderButton;
    bool saveImageButton;
    bool mouseOrbit;

    //orbit controls
    float orbitRadius = 0.05f;
    float orbitSpeed = 1.0f;

    //high precision fractal settings
    Image highPrecisionImage;
    Texture2D highPrecisionTexture;
    int percentLoaded = 0;
    bool shouldLoadHighPrecisionTexture = false;
    bool isLoadingHighPrecisionFractal = false;

    std::string realCoordinate = "0";
    std::string imaginaryCoordinate = "0";
    std::string zoomInput = "2.0";

    struct DVector2 {
        double x;
        double y;
    };

    double DVector2DotProduct(DVector2 v1, DVector2 v2) { return v1.x * v2.x + v1.y * v2.y; };
    DVector2 DVector2Add(DVector2 v1, DVector2 v2) { return DVector2{ v1.x + v2.x, v1.y + v2.y }; };

    // fractal functions
    void setFractal(Shader& shader);
    
    template<typename T>
    T mapToReal(int x, T minR, T maxR);

    template<typename T>
    T mapToImaginary(int y, T minI, T maxI);

    void renderRealTimeFractal();
    void drawCalculatedFractalToImage();
    void drawFractalLoadPercentage();

    Vector2 mandelbrotFormula(Vector2 z, Vector2 c);
    DVector2 preciseMandelbrotFormula(DVector2 z, DVector2 c);
    Vector2 burningshipFormula(Vector2 z, Vector2 c);
    Vector2 tricornFormula(Vector2 z, Vector2 c);

    bool highPrecisionMode = false;
    bool juliaFrozen = false;

};

