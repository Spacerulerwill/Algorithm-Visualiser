#include "FractalVisualiser.h"
#include "imgui.h"
#include "imgui_impl_raylib.h"
#include "imgui_internal.h"
#include "rlImGui.h"
#include <iostream>
#include <sstream>

#define GLSL_VERSION 450

FractalVisualiser::FractalVisualiser(float xRatio, float yRatio, float widthRatio, float heightRatio) : ScreenElement(xRatio, yRatio, widthRatio, heightRatio)
{
	mandelbrot = LoadShader(0, TextFormat("mandelbrot.fs", GLSL_VERSION));
    burningShip = LoadShader(0, TextFormat("burningship.fs", GLSL_VERSION));
    julia = LoadShader(0, TextFormat("julia.fs", GLSL_VERSION));
    tricorn = LoadShader(0, TextFormat("tricorn.fs", GLSL_VERSION));

    target = LoadRenderTexture(getWidth(), getHeight());

    setFractal(mandelbrot);

    color_1 = 1.0f;
    color_2 = 1.0f;
    color_3 = 1.0f;
    color_4 = 1.0f;
}

void FractalVisualiser::draw()
{

    //enable drawing to texture, clear its background
    BeginTextureMode(target);       // Enable drawing to texture
    ClearBackground(RAYWHITE);  // Clear texture background
    EndTextureMode();               // End drawing to texture (now we have a texture available for next passes)


    // Render generated texture using selected postprocessing shader
    BeginShaderMode(*activeFractal);
    // NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
    DrawTexturePro(target.texture, Rectangle{ getX(), getY(), (float)target.texture.width, (float)target.texture.height}, Rectangle{getX(), getY(), getWidth(), getHeight()}, Vector2{0, 0}, 0.0f, WHITE);

    EndShaderMode();

    //Draw Text
    float font = (GetScreenWidth()) / 50;
    std::stringstream stream;
    stream << "Focused on point: ";
    stream << -location[0] << " + " << -location[1] << "i";
    std::string s = stream.str();
    const char* text = s.c_str();
    DrawText(text, 0, 0, font, WHITE);

    //Draw UI

    ImGui::Begin("Fractal Settings", NULL, ImGuiWindowFlags_None);
    fractalSelector = ImGui::Combo("Fractal", &selectedFractal, fractals, 4);
    ImGui::SliderInt("Iterations", &iterations, 0, 1000, "% .3f");
    ImGui::SliderFloat("Color 1", &color_1, 0.0f, 1.0f, "% .3f");
    ImGui::SliderFloat("Color 2", &color_2, 0.0f, 1.0f, "% .3f");
    ImGui::SliderFloat("Color 3", &color_3, 0.0f, 1.0f, "% .3f");
    ImGui::SliderFloat("Color 4", &color_4, 0.0f, 1.0f, "% .3f");
    ImGui::End();

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

}
	
void FractalVisualiser::keyEvents()
{
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
    if (IsKeyDown(KEY_EQUAL) && zoom >= 0) {
        zoom -= 0.01 * zoom;
    }
    if (IsKeyDown(KEY_MINUS)) {
        zoom += 0.01 * zoom;
    }

    if (fractalSelector) {
        switch (selectedFractal) {
        case 0: {
            setFractal(mandelbrot);
            break;
        }
        case 1: {
            setFractal(julia);
            break;
        }
        case 2: {
            setFractal(burningShip);
            break;
        }
        case 3: {
            setFractal(tricorn);
            break;
        }
        }
    }
}

void FractalVisualiser::setFractal(Shader& fractal)
{
    activeFractal = &fractal;
    color_1Loc = GetShaderLocation(fractal, "color_1");
    color_2Loc = GetShaderLocation(fractal, "color_2");
    color_3Loc = GetShaderLocation(fractal, "color_3");
    color_4Loc = GetShaderLocation(fractal, "color_4");
    iterationsLoc = GetShaderLocation(fractal, "iterations");
    resolutionLoc = GetShaderLocation(fractal, "resolution");
    locationLoc = GetShaderLocation(fractal, "location");
    zoomLoc = GetShaderLocation(fractal, "zoom");
}
