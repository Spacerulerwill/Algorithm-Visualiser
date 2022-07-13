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
#include <functional>

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

float FractalVisualiser::mapToReal(float x, float minR, float maxR) {
    float range = maxR - minR;
    return x * (range / getWidth()) + minR;
}

float FractalVisualiser::mapToImaginary(float y, float minI, float maxI) {
    float range = maxI - minI;
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

    if (!highPrecisionMode) {
        if (ImGui::Checkbox("Julia Set", &juliaMode)) {
            stabilityVisualiser = false;
        }

        if (ImGui::Checkbox("Stabiliy Visualiser", &stabilityVisualiser)) {
            juliaMode = false;
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
    float font = (GetScreenWidth()) / 50;
    std::stringstream stream;
    stream << "Focused on point: ";

    if (juliaMode) {
        stream << mousePos.x << "  + " << -mousePos.y << "i\n";
        stream << "S - save image\n";
        if (juliaFrozen) {
            stream << "F - unfreeze Julia Set";
        }
        else {
            stream << "F - freeze Julia Set";
        }
    }
    else {
        stream << location[0] << " + " << location[1] << "i\n";
        stream << "+/= Zoom\nArrow keys - Pan\n";
        stream << zoom << "x zoom\n";
        stream << "S - save image\nR - reset\nJ - Julia Set\n";
    }

    std::string s = stream.str();
    const char* text = s.c_str();
    DrawText(text, 0, 0, font, WHITE);

    //update shader values
    resolution[0] = getWidth();
    resolution[1] = getHeight();

    SetShaderValue(*activeFractal, resolutionLoc, &resolution, SHADER_UNIFORM_VEC2);

    //map mouse coordinate to imaginary point
    float minR = ((-0.5 * getWidth() / getHeight()) * zoom) + location[0];
    float maxR = ((0.5 * getWidth() / getHeight()) * zoom) + location[0];
    float minI = -0.5 * zoom - location[1];
    float maxI = 0.5 * zoom - location[1];

    //set shader values
    if (!juliaFrozen || (juliaFrozen && stabilityVisualiser)) {
        //set mousepos shader value
        mousePos = Vector2{ mapToReal(GetMousePosition().x, minR, maxR), mapToImaginary(GetMousePosition().y, minI, maxI) };
    }

    SetShaderValue(*activeFractal, mousePosLoc, &mousePos, SHADER_UNIFORM_VEC2);
    SetShaderValue(*activeFractal, juliaModeLoc, &juliaMode, SHADER_UNIFORM_INT); 
    SetShaderValue(*activeFractal, locationLoc, &location, SHADER_UNIFORM_VEC2);
    SetShaderValue(*activeFractal, zoomLoc, &zoom, SHADER_UNIFORM_FLOAT); 
    SetShaderValue(*activeFractal, color_1Loc, &color_1, SHADER_UNIFORM_FLOAT);
    SetShaderValue(*activeFractal, color_2Loc, &color_2, SHADER_UNIFORM_FLOAT);
    SetShaderValue(*activeFractal, color_3Loc, &color_3, SHADER_UNIFORM_FLOAT);
    SetShaderValue(*activeFractal, color_4Loc, &color_4, SHADER_UNIFORM_FLOAT);
    SetShaderValue(*activeFractal, iterationsLoc, &iterations, SHADER_UNIFORM_INT);

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
            int x = 0;
            int y = 0;

            DrawCircle(x, y, 5, RED);
        }
    }
}

Vector2 FractalVisualiser::mandelbrotFormula(Vector2 z, Vector2 c)
{
    float temp = z.x;
    z.x = z.x * z.x - z.y * z.y;
    z.y = 2.0 * temp * z.y;
    z = Vector2Add(z, c);
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
    float real = std::stof(realCoordinate);
    float imag = std::stof(imaginaryCoordinate);

    // convert zoom to coordinate
    float zoom = std::stof(zoomInput);

    isLoadingHighPrecisionFractal = true;
    highPrecisionImage = GenImageColor((int)getWidth(), (int)getHeight(), BLANK);
    //map mouse coordinate to imaginary point
    float minR = ((-0.5 * getWidth() / getHeight()) * zoom) + real;
    float maxR = ((0.5 * getWidth() / getHeight()) * zoom) + real;
    float minI = -0.5 * zoom - imag;
    float maxI = 0.5 * zoom - imag;

    int totalPixels = getWidth() * getHeight();
    int column = getHeight();
    int onePercent = 0.01 * totalPixels;

    for (int x = 0; x < getWidth(); x++) {
        for (int y = 0; y < getHeight(); y++) {
            Vector2 c = { mapToReal(x, minR, maxR), mapToImaginary(y, minI, maxI) };
            Vector2 z = { 0.0f, 0.0f };

            int i;
            for (i = 0; i < iterations; i++) {        
                if (activeFractal == &mandelbrot) {
                    //square
                    z = mandelbrotFormula(z, c);
                }
                else if (activeFractal == &burningShip) {
                    z = burningshipFormula(z, c);
                }
                else if (activeFractal == &tricorn) {
                    z = tricornFormula(z, c);
                }

                if (Vector2DotProduct(z, z) > 4.0) {
                    break;
                }
            }
            //convert RGB to HSV
            Color color = ColorFromHSV((i / float(iterations))*360, 1.0, 1.0);
            ImageDrawPixel(&highPrecisionImage, x, y, color);
            int total = x * column + y;

            //percentage
            if (total % onePercent == 0) {
                percentLoaded = (total/(float)totalPixels) * 100;
            }
        }
    }
    shouldLoadHighPrecisionTexture = true;
    isLoadingHighPrecisionFractal = false;
    percentLoaded = 100;
}
	
void FractalVisualiser::keyEvents()
{
    //save fractal image
    if (IsKeyPressed(KEY_S)) {
        //string stream for name
        std::stringstream stream;
        if (juliaMode) {
            stream << fractals[selectedFractal] << " Julia Set at " << mousePos.x << " + " << -mousePos.y << "i.png";
        }
        else {
            stream << fractals[selectedFractal] << " at " << location[0] << " + " << location[1] << "i.png";
        }

        std::string s = stream.str();
        const char* name = s.c_str();

        //save
        Image image = LoadImageFromScreen();
        ExportImage(image, name);
        UnloadImage(image);
    }
    if (!highPrecisionMode) {
        if (IsKeyPressed(KEY_R)) {
            //change zoom to default and move back to center
            location[0] = 0;
            location[1] = 0;
            zoom = 2.0f;
        }
        if (IsKeyPressed(KEY_J)) {
            juliaMode = !juliaMode;
        }
        if (IsKeyPressed(KEY_F)) {
            juliaFrozen = !juliaFrozen;
        }
        if (IsKeyDown(KEY_DOWN)) {
            location[1] -= 0.01 * zoom;
        }
        if (IsKeyDown(KEY_UP)) {
            location[1] += 0.01 * zoom;
        }
        if (IsKeyDown(KEY_LEFT)) {
            location[0] -= 0.01 * zoom;
        }
        if (IsKeyDown(KEY_RIGHT)) {
            location[0] += 0.01 * zoom;
        }
        //zooming in
        if (IsKeyDown(KEY_EQUAL) && zoom >= 0) {
            zoom -= 0.01 * zoom;
        }
        if (IsKeyDown(KEY_MINUS)) {
            zoom += 0.01 * zoom;
        }
    }

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