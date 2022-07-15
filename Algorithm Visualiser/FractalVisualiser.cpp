#include "FractalVisualiser.h"
#include "imgui.h"
#include "imgui_impl_raylib.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"
#include "rlImGui.h"
#include "raymath.h"
#include <sstream>
#include <thread>
#include <iostream>
#include <string>

#define GLSL_VERSION 450

FractalVisualiser::FractalVisualiser(float xRatio, float yRatio, float widthRatio, float heightRatio) : ScreenElement(xRatio, yRatio, widthRatio, heightRatio)
{
	mandelbrot = LoadShader(0, TextFormat("shaders/mandelbrot.fs", GLSL_VERSION));
    burningShip = LoadShader(0, TextFormat("shaders/burningship.fs", GLSL_VERSION));
    tricorn = LoadShader(0, TextFormat("shaders/tricorn.fs", GLSL_VERSION));

    Image imBlank = GenImageColor((int)getWidth(), (int)getHeight(), BLANK);
    realTimeTexture = LoadTextureFromImage(imBlank);  // Load blank texture to fill on shader

    UnloadImage(imBlank);
    setFractal(mandelbrot);
}   

template<typename T>
T FractalVisualiser::mapToReal(int x, T minR, T maxR) {
    T range = maxR - minR;
    return x * (range / getWidth()) + minR;
}

template<typename T>
T FractalVisualiser::mapToImaginary(int y, T minI, T maxI) {
    T range = maxI - minI;
    return y * (range / getHeight()) + minI;
}

void FractalVisualiser::draw() {

    ImGui::Begin("Fractal Settings", NULL, ImGuiWindowFlags_None);
    fractalSelector = ImGui::Combo("Fractal", &selectedFractal, fractals, numFractals);
    ImGui::SliderInt("Iterations", &iterations, 0, 1000, "% .3f");

    if (highPrecisionMode) {
        realInputBox = ImGui::InputText("Real Number", &realCoordinate);
        imaginaryInputBox = ImGui::InputText("Imaginary Number", &imaginaryCoordinate);
        zoomInputBox = ImGui::InputText("Zoom", &zoomInput);
    }

    //floats for colors
    ImGui::SliderFloat("Color 1", &color_1, 0.0f, 1.0f, "% .3f");
    ImGui::SliderFloat("Color 2", &color_2, 0.0f, 1.0f, "% .3f");
    ImGui::SliderFloat("Color 3", &color_3, 0.0f, 1.0f, "% .3f");
    ImGui::SliderFloat("Color 4", &color_4, 0.0f, 1.0f, "% .3f");

    colorSelector = ImGui::Combo("Color Preset", &selectedColorPreset, colorPresetNames, numColors);
    saveImageButton = ImGui::Button("Save Image");

    if (!highPrecisionMode) {
        if (ImGui::Checkbox("Julia Set", &juliaMode)) {
            stabilityVisualiser = false;
        }

        if (ImGui::Checkbox("Stabiliy Visualiser", &stabilityVisualiser)) {
            juliaMode = false;
        }

        if (juliaMode) {
            ImGui::Checkbox("Mouse Orbit", &mouseOrbit);
            if (mouseOrbit) {
                ImGui::SliderFloat("Orbital Radius", &orbitRadius, 0.05f, 1.0f);
                ImGui::SliderFloat("Orbit Speed", &orbitSpeed, 1.0f, 10.0f);
            }
        }
    }
    else {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, isLoadingHighPrecisionFractal); // disable controls if sorting
        renderButton = ImGui::Button("Render Image");
        ImGui::PopItemFlag();

        if (renderButton) {
            std::thread th(&FractalVisualiser::drawCalculatedFractalToImage, this);
            th.detach();
        }
     }    

    if (ImGui::Checkbox("High Precision Mode (Non Real-Time)", &highPrecisionMode)) {
        juliaMode = false;
        stabilityVisualiser = false;
    }

    ImGui::End();

    //draw high precision mode
    if (highPrecisionMode){
        DrawTexturePro(highPrecisionTexture, Rectangle{ getX(), getY(), (float)highPrecisionTexture.width, (float)highPrecisionTexture.height }, Rectangle{ getX(), getY(), getWidth(), getHeight() }, Vector2{ 0, 0 }, 0.0f, WHITE);
        drawFractalLoadPercentage();
    }
    //draw real time mode
    else {
        renderRealTimeFractal();
    }
}

void FractalVisualiser::drawFractalLoadPercentage() {
    std::stringstream stream;
    stream << percentLoaded << "%";
    std::string s = stream.str();
    const char* text = s.c_str();
    DrawText(text, 0, 0, 50, WHITE);
}

void FractalVisualiser::renderRealTimeFractal()
{  
    BeginShaderMode(*activeFractal);
    DrawTexturePro(realTimeTexture, Rectangle{ getX(), getY(), (float)realTimeTexture.width, (float)realTimeTexture.height }, Rectangle{ getX(), getY(), getWidth(), getHeight() }, Vector2{ 0, 0 }, 0.0f, WHITE);
    EndShaderMode();
    
    //Draw Text
    float font = (GetScreenWidth()) / 50.0f;
    std::stringstream stream;
    stream << "Focused on point: ";

    if (juliaMode) {
        stream << mousePos.x << "  + " << -mousePos.y << "i\n";
        if (juliaFrozen) {
            stream << "F - unfreeze Julia Set";
        }
        else {
            stream << "F - freeze Julia Set";
        }
    }
    else {
        stream << location.x << " + " << location.y << "i\n";
        stream << "+/= Zoom\nArrow keys - Pan\n";
        stream << zoom << "x zoom\n";
        stream << "R - reset\nJ - Julia Set\n";
    }

    std::string s = stream.str();
    const char* text = s.c_str();
    DrawText(text, 0, 0, font, WHITE);

    //update shader values
    resolution[0] = getWidth();
    resolution[1] = getHeight();

    SetShaderValue(*activeFractal, resolutionLoc, &resolution, SHADER_UNIFORM_VEC2);

    //map mouse coordinate to imaginary point
    float minR = ((-0.5 * getWidth() / getHeight()) * zoom) + location.x;
    float maxR = ((0.5 * getWidth() / getHeight()) * zoom) + location.x;
    float minI = -0.5 * zoom - location.y;
    float maxI = 0.5 * zoom - location.y;

    //set mousePos
    if (!juliaFrozen || (juliaFrozen && stabilityVisualiser)) {
        //set mousepos shader value
        mousePos = Vector2{ mapToReal(GetMousePosition().x, minR, maxR), mapToImaginary(GetMousePosition().y, minI, maxI) };
        SetShaderValue(*activeFractal, mousePosLoc, &mousePos, SHADER_UNIFORM_VEC2);
    }
    if (mouseOrbit && juliaMode) {
        Vector2 newMousePos = mousePos;
        newMousePos.x += sin(orbitSpeed * GetTime()) * orbitRadius;
        newMousePos.y += cos(orbitSpeed * GetTime()) * orbitRadius;
        SetShaderValue(*activeFractal, mousePosLoc, &newMousePos, SHADER_UNIFORM_VEC2);
    }

    //stability visualiser mode
    if (stabilityVisualiser) {
        Vector2 z = { 0, 0 };
        for (int i = 0; i < 50; i++) {
            //start at zero, do appropriate set formula and add mousepos;
            if (activeFractal == &mandelbrot) {
                z = mandelbrotFormula(z, mousePos);
            }
            else if (activeFractal == &tricorn) {
                z = tricornFormula(z, mousePos);
            }
            else if (activeFractal == &burningShip) {
                z = burningshipFormula(z, mousePos);
            }

            //convert back to screen space coordinates
            int x = z.x / ((maxR - minR) / getWidth()) - minR + getWidth() / 2;;
            int y = z.y / ((maxI - minI) / getHeight()) - minI + getHeight() / 2;

            DrawCircle(x, y, 5, RED);
        }
    }
  
    SetShaderValue(*activeFractal, juliaModeLoc, &juliaMode, SHADER_UNIFORM_INT); 
    SetShaderValue(*activeFractal, locationLoc, &location, SHADER_UNIFORM_VEC2);
    SetShaderValue(*activeFractal, zoomLoc, &zoom, SHADER_UNIFORM_FLOAT); 
    SetShaderValue(*activeFractal, color_1Loc, &color_1, SHADER_UNIFORM_FLOAT);
    SetShaderValue(*activeFractal, color_2Loc, &color_2, SHADER_UNIFORM_FLOAT);
    SetShaderValue(*activeFractal, color_3Loc, &color_3, SHADER_UNIFORM_FLOAT);
    SetShaderValue(*activeFractal, color_4Loc, &color_4, SHADER_UNIFORM_FLOAT);
    SetShaderValue(*activeFractal, iterationsLoc, &iterations, SHADER_UNIFORM_INT);
}

Vector2 FractalVisualiser::mandelbrotFormula(Vector2 z, Vector2 c)
{
    float temp = z.x;
    z.x = z.x * z.x - z.y * z.y;
    z.y = 2.0 * temp * z.y;
    z = Vector2Add(z, c);
    return z;
}

FractalVisualiser::DVector2 FractalVisualiser::preciseMandelbrotFormula(DVector2 z, DVector2 c)
{
    double temp = z.x;
    z.x = z.x * z.x - z.y * z.y;
    z.y = 2.0 * temp * z.y;
    z = DVector2Add(z, c);
    return z;
}

Vector2 FractalVisualiser::burningshipFormula(Vector2 z, Vector2 c)
{
    float temp = abs(z.x);
    z.x = abs(z.x * z.x) - abs(z.y * z.y);
    z.y = 2.0 * temp * abs(z.y);
    z = Vector2Add(z, c);
    return z;
}

Vector2 FractalVisualiser::tricornFormula(Vector2 z, Vector2 c)
{
    z = { z.x, -z.y };
    float temp = z.x;
    z.x = z.x * z.x - z.y * z.y;
    z.y = 2.0 * temp * z.y;
    z = Vector2Add(z, c);
    return z;
}

// high precision mode (runs with arbitrary precision floats)
void FractalVisualiser::drawCalculatedFractalToImage()
{
    
    // convert coordinate to string to coordinates and invert the signs
    double real = std::stod(realCoordinate);
    double imag = std::stod(imaginaryCoordinate);

    //convert zoom
    double zoom = std::stod(zoomInput);

    isLoadingHighPrecisionFractal = true;
    highPrecisionImage = GenImageColor((int)getWidth(), (int)getHeight(), BLANK);

    // map to complex point
    double minR = ((-0.5 * getWidth() / getHeight()) * zoom) + real;
    double maxR = ((0.5 * getWidth() / getHeight()) * zoom) + real;
    double minI = -0.5 * zoom - imag;
    double maxI = 0.5 * zoom - imag;

    int totalPixels = getWidth() * getHeight();
    int column = getHeight();
    int onePercent = 0.01 * totalPixels;

    for (int x = 0; x < getWidth(); x++) {
        for (int y = 0; y < getHeight(); y++) {
            DVector2 c = { mapToReal(x, minR, maxR), mapToImaginary(y, minI, maxI) };
            DVector2 z = { 0.0, 0.0 };

            int i;
            for (i = 0; i < iterations; i++) {
                if (activeFractal == &mandelbrot) {
                    //square
                    z = preciseMandelbrotFormula(z, c);
                }
                if (DVector2DotProduct(z, z) > 4.0) {
                    break;
                }
            }
            //convert RGB to HSV
            Color color = ColorFromHSV((i / float(iterations)) * 360, 1.0, 1.0);
            ImageDrawPixel(&highPrecisionImage, x, y, color);
            int total = x * column + y;

            //percentage
            if (total % onePercent == 0) {
                percentLoaded = (total / (float)totalPixels) * 100;
            }
        }
    }
    shouldLoadHighPrecisionTexture = true;
    isLoadingHighPrecisionFractal = false;
    percentLoaded = 100;
}
	
void FractalVisualiser::keyEvents()
{ 
    if (!highPrecisionMode) {
        if (IsKeyPressed(KEY_R)) {
            //change zoom to default and move back to center
            location.x = 0;
            location.y = 0;
            zoom = 2.0f;
        }
        if (IsKeyPressed(KEY_J)) {
            juliaMode = !juliaMode;
        }
        if (IsKeyPressed(KEY_F)) {
            juliaFrozen = !juliaFrozen;
        }
        if (IsKeyDown(KEY_DOWN)) {
            location.y -= 0.01 * zoom;
        }
        if (IsKeyDown(KEY_UP)) {
            location.y += 0.01 * zoom;
        }
        if (IsKeyDown(KEY_LEFT)) {
            location.x -= 0.01 * zoom;
        }
        if (IsKeyDown(KEY_RIGHT)) {
            location.x += 0.01 * zoom;
        }
        //zooming in
        if (IsKeyDown(KEY_EQUAL) && zoom >= 0) {
            zoom -= 0.01 * zoom;
        }
        if (IsKeyDown(KEY_MINUS)) {
            zoom += 0.01 * zoom;
        }
    }
}

void FractalVisualiser::events()
{
    //selecting fractal from menu
    if (fractalSelector) {
        switch (selectedFractal) {
        case 0: {
            setFractal(mandelbrot);
            break;
        }
        case 1: {
            setFractal(burningShip);
            break;
        }
        case 2: {
            setFractal(tricorn);
            break;
        }
        }
    }

    //changing colors
    if (colorSelector) {
        color_1 = colorPresets[selectedColorPreset][0];
        color_2 = colorPresets[selectedColorPreset][1];
        color_3 = colorPresets[selectedColorPreset][2];
        color_4 = colorPresets[selectedColorPreset][3];
    }

    // load high precision texture when flag activated
    if (shouldLoadHighPrecisionTexture) {
        highPrecisionTexture = LoadTextureFromImage(highPrecisionImage);
        shouldLoadHighPrecisionTexture = false;
    }

    if (saveImageButton) {
        //string stream for name
        std::stringstream stream;
        if (juliaMode) {
            stream << fractals[selectedFractal] << " Julia Set at " << mousePos.x << " + " << -mousePos.y << "i.png";
        }
        else {
            stream << fractals[selectedFractal] << " at " << location.x << " + " << location.y << "i.png";
        }

        std::string s = stream.str();
        const char* name = s.c_str();

        //save
        Image image = LoadImageFromScreen();
        std::cout << "Exported\n";
        //ExportImage(image, name);
        //UnloadImage(image);
    }
}

void FractalVisualiser::setFractal(Shader& fractal)
{
    //uniform locations
    if (&fractal != activeFractal) {
        activeFractal = &fractal;
        resolutionLoc = GetShaderLocation(fractal, "resolution");
        locationLoc = GetShaderLocation(fractal, "location");
        mousePosLoc = GetShaderLocation(fractal, "mousePos");
        juliaModeLoc = GetShaderLocation(fractal, "juliaMode");
        zoomLoc = GetShaderLocation(fractal, "zoom");
        color_1Loc = GetShaderLocation(fractal, "color_1");
        color_2Loc = GetShaderLocation(fractal, "color_2");
        color_3Loc = GetShaderLocation(fractal, "color_3");
        color_4Loc = GetShaderLocation(fractal, "color_4");
        iterationsLoc = GetShaderLocation(fractal, "iterations");
    }
}

FractalVisualiser::~FractalVisualiser()
{
    UnloadTexture(realTimeTexture);
    UnloadTexture(highPrecisionTexture);
    UnloadImage(highPrecisionImage);
    UnloadShader(mandelbrot);    
    UnloadShader(burningShip);
    UnloadShader(tricorn);
}