#include "FractalVisualiser.h"
#include "imgui.h"
#include "imgui_impl_raylib.h"
#include "imgui_internal.h"
#include "rlImGui.h"
#include "raymath.h"
#include <sstream>
#include <thread>
#include <iostream>

#define GLSL_VERSION 450

FractalVisualiser::FractalVisualiser(float xRatio, float yRatio, float widthRatio, float heightRatio) : ScreenElement(xRatio, yRatio, widthRatio, heightRatio)
{
	mandelbrot = LoadShader(0, TextFormat("shaders/mandelbrot.fs", GLSL_VERSION));
    burningShip = LoadShader(0, TextFormat("shaders/burningship.fs", GLSL_VERSION));
    tricorn = LoadShader(0, TextFormat("shaders/tricorn.fs", GLSL_VERSION));

    Image imBlank = GenImageColor(getWidth(), getHeight(), BLANK);
    texture = LoadTextureFromImage(imBlank);  // Load blank texture to fill on shader
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


void FractalVisualiser::draw()
{  
    BeginShaderMode(*activeFractal);
    DrawTexturePro(texture, Rectangle{ getX(), getY(), (float)texture.width, (float)texture.height }, Rectangle{ getX(), getY(), getWidth(), getHeight() }, Vector2{ 0, 0 }, 0.0f, WHITE);
    EndShaderMode();
    
    //Draw Text
    float font = (GetScreenWidth()) / 50;
    std::stringstream stream;
    stream << "Focused on point: ";

    if (juliaMode) {
        stream << mousePos.x << "  + " << -mousePos.y << "i\n";
        stream << "S - save image\n";
    }
    else {
        stream << "+/= Zoom\nArrow keys - Pan\n";
        stream << zoom << "x zoom\n";
        stream << -location[0] << " + " << -location[1] << "i\n";
        stream << "R - reset\nJ - Julia Set\n";
    }

    std::string s = stream.str();
    const char* text = s.c_str();
    DrawText(text, 0, 0, font, WHITE);

    //Draw UI
    ImGui::Begin("Fractal Settings", NULL, ImGuiWindowFlags_None);
    fractalSelector = ImGui::Combo("Fractal", &selectedFractal, fractals, 3);
    ImGui::SliderInt("Iterations", &iterations, 0, 1000, "% .3f");
    
    //float conversion for slider bar
    ImGui::SliderFloat("Color 1", &color_1, 0.0f, 1.0f, "% .3f");
    ImGui::SliderFloat("Color 2", &color_2, 0.0f, 1.0f, "% .3f");
    ImGui::SliderFloat("Color 3", &color_3, 0.0f, 1.0f, "% .3f");
    ImGui::SliderFloat("Color 4", &color_4, 0.0f, 1.0f, "% .3f");

    colorSelector = ImGui::Combo("Color Preset", &selectedColorPreset, colorPresetNames, 4);

    if (ImGui::Checkbox("Julia Set", &juliaMode)) {
        stabilityVisualiser = false;
    }

    if (ImGui::Checkbox("Stabiliy Visualiser", &stabilityVisualiser)) {
        juliaMode = false;
    }

    //std::cout << juliaMode << " " << stabilityVisualiser << "\n";

    ImGui::End();

    SetShaderValue(*activeFractal, juliaModeLoc, &juliaMode, SHADER_UNIFORM_INT);
    SetShaderValue(*activeFractal, color_1Loc, &color_1, SHADER_UNIFORM_FLOAT);
    SetShaderValue(*activeFractal, color_2Loc, &color_2, SHADER_UNIFORM_FLOAT);
    SetShaderValue(*activeFractal, color_3Loc, &color_3, SHADER_UNIFORM_FLOAT);
    SetShaderValue(*activeFractal, color_4Loc, &color_4, SHADER_UNIFORM_FLOAT);
    SetShaderValue(*activeFractal, iterationsLoc, &iterations, SHADER_UNIFORM_INT);
    SetShaderValue(*activeFractal, locationLoc, &location, SHADER_UNIFORM_VEC2);
    SetShaderValue(*activeFractal, zoomLoc, &zoom, SHADER_UNIFORM_FLOAT);
    resolution[0] = getWidth();
    resolution[1] = getHeight();
    SetShaderValue(*activeFractal, resolutionLoc, &resolution, SHADER_UNIFORM_VEC2);

    //map mouse coordinate to mandelbrot point
    float minR = ((-0.5 * getWidth() / getHeight()) * zoom) - location[0];
    float maxR = ((0.5 * getWidth() / getHeight()) * zoom) - location[0];
    float minI = -0.5 * zoom + location[1];
    float maxI = 0.5 * zoom + location[1];

    mousePos = Vector2{ mapToReal(GetMousePosition().x, minR, maxR), mapToImaginary(GetMousePosition().y, minI, maxI) };
    

    SetShaderValue(*activeFractal, mousePosLoc, &mousePos, SHADER_UNIFORM_VEC2);

    //stability visualiser mode
    if (stabilityVisualiser) {
        Vector2 z = { 0, 0 };
        for (int i = 0; i < 50; i++) {

            //start at zero, do appropriate set formula and add mousepos;
            if (activeFractal == &mandelbrot) {
                float temp = z.x;
                z.x = z.x * z.x - z.y * z.y;
                z.y = 2.0 * temp * z.y;
                z = Vector2Add(z, mousePos);
            }
            else if (activeFractal == &tricorn) {
                z = { z.x, -z.y };
                float temp = z.x;
                z.x = z.x * z.x - z.y * z.y;
                z.y = 2.0 * temp * z.y;
                z = Vector2Add(z, mousePos);

            }
            else if (activeFractal == &burningShip) {
                float temp = abs(z.x);
                z.x = abs(z.x * z.x) - abs(z.y * z.y);
                z.y = 2.0 * temp * abs(z.y);
                z = Vector2Add(z, mousePos);
            }
         
            //convert back to screenspace and draw a circle at each point with connected lines;
            int x = ((z.x + location[0]) * (getWidth() / (maxR - minR))) + getWidth() / 2;
            int y = ((z.y - location[1]) * (getHeight() / (maxI - minI))) + getHeight() / 2;

            DrawCircle(x, y, 5, RED);
        }
    }

    //set mousepos shader value
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
            stream << fractals[selectedFractal] << " at " << - location[0] << " + " << -location[1] << "i.png";
        }

        std::string s = stream.str();
        const char* name = s.c_str();

        //save
        Image image = LoadImageFromScreen();
        ExportImage(image, name);
        UnloadImage(image);
    }
    if (IsKeyPressed(KEY_R)) {
        //change zoom to default and move back to center
        location[0] = 0;
        location[1] = 0;
        zoom = 2.0f;
    }
    if (IsKeyPressed(KEY_J)) {
        juliaMode = !juliaMode;
    }
    if (IsKeyPressed(KEY_ENTER) && juliaMode) {
        std::cout << "bruh";
    }
    //panning controls
    if (IsKeyDown(KEY_DOWN)) {
        location[1] += 0.01 * zoom;
    }
    if (IsKeyDown(KEY_UP)) {
        location[1] -= 0.01 * zoom;
    }
    if (IsKeyDown(KEY_LEFT)) {
        location[0] += 0.01 * zoom;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        location[0] -= 0.01 * zoom;
    }

    //zooming in
    if (IsKeyDown(KEY_EQUAL) && zoom >= 0) {
        zoom -= 0.01 * zoom;
    }
    if (IsKeyDown(KEY_MINUS)) {
        zoom += 0.01 * zoom;
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
}

void FractalVisualiser::setFractal(Shader& fractal)
{
    //uniform locations
    activeFractal = &fractal;
    zoomLoc = GetShaderLocation(fractal, "zoom");
    color_1Loc = GetShaderLocation(fractal, "color_1");
    color_2Loc = GetShaderLocation(fractal, "color_2");
    color_3Loc = GetShaderLocation(fractal, "color_3");
    color_4Loc = GetShaderLocation(fractal, "color_4");
    iterationsLoc = GetShaderLocation(fractal, "iterations");
    resolutionLoc = GetShaderLocation(fractal, "resolution");
    locationLoc = GetShaderLocation(fractal, "location");
    mousePosLoc = GetShaderLocation(fractal, "mousePos");
    juliaModeLoc = GetShaderLocation(fractal, "juliaMode"); 
}

FractalVisualiser::~FractalVisualiser()
{
    UnloadTexture(texture);
    UnloadShader(mandelbrot);    
    UnloadShader(burningShip);
    UnloadShader(tricorn);
}