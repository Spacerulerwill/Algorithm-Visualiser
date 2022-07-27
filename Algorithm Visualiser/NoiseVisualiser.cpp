#include "NoiseVisualiser.h"
#include "imgui.h"
#include <iostream>
#include <chrono>
#include <sstream>
#include <thread>

#define GLSL_VERSION 450

NoiseVisualiser::NoiseVisualiser(float xRatio, float yRatio, float widthRatio, float heightRatio) : ScreenElement(xRatio, yRatio, widthRatio, heightRatio)
{
	//initialise random seed
	srand(time(NULL));

	randomNoise = LoadShader(0, TextFormat("shaders/noise/randomnoise.fs", GLSL_VERSION));
	valueNoise = LoadShader(0, TextFormat("shaders/noise/valuenoise.fs", GLSL_VERSION));
	perlinNoise = LoadShader(0, TextFormat("shaders/noise/perlinnoise.fs", GLSL_VERSION));
	cellularNoise = LoadShader(0, TextFormat("shaders/noise/cellularnoise.fs", GLSL_VERSION));

	Image imBlank = GenImageColor((int)getWidth(), (int)getHeight(), BLANK);
	noiseTex = LoadTextureFromImage(imBlank);  // Load blank texture to fill on shader
	UnloadImage(imBlank);

	setNoise(randomNoise);

	// start seed thread
	std::thread th(&NoiseVisualiser::seedLoop, this);
	th.detach();

}

NoiseVisualiser::~NoiseVisualiser()
{
	UnloadTexture(noiseTex);
	UnloadShader(randomNoise);
}

void NoiseVisualiser::draw()
{

	// draw noise settings
	ImGui::SetNextWindowSize({ 0.0f, 0.0f });
	ImGui::Begin("Noie Settings", NULL, ImGuiWindowFlags_None);

	// seed label
	std::stringstream stream;
	stream << "Seed: ";
	stream << seed;
	std::string str = stream.str();
	const char* c_str = str.c_str();
	ImGui::Text(c_str);

	ImGui::SliderInt("Seed Delay (ms)", &seedDelayMS, 1, 1000);

	// noise selector
	noiseSelector = ImGui::Combo("Noise Type", &selectedNoise, noises, numNoises);

	// dimension selector
	dimensionSelector = ImGui::Combo("Dimension", &selectedDimension, dimensions, 3);

	// static checkbox
	ImGui::Checkbox("static", &isStatic);
	ImGui::End();
	
	// if in 2d mode, use shader to render
	if (selectedDimension == 1) {
		BeginShaderMode(*activeNoise);
		DrawTexturePro(noiseTex, Rectangle{ getX(), getY(), (float)noiseTex.width, (float)noiseTex.height }, Rectangle{ getX(), getY(), getWidth(), getHeight() }, Vector2{ 0, 0 }, 0.0f, WHITE);
		EndShaderMode();
	}

	if (!isStatic) {
		SetShaderValue(*activeNoise, seedLoc, &seed, SHADER_UNIFORM_INT);
	}

	resolution = { getWidth(), getHeight() };

	SetShaderValue(*activeNoise, zoomLoc, &zoom, SHADER_UNIFORM_FLOAT);
	SetShaderValue(*activeNoise, locationLoc, &location, SHADER_UNIFORM_VEC2);
	SetShaderValue(*activeNoise, resolutionLoc, &resolution, SHADER_UNIFORM_VEC2);
}

void NoiseVisualiser::keyEvents()
{
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
	if (IsKeyPressed(KEY_R)) {
		location = { 0.0f, 0.0f };
	}
}

void NoiseVisualiser::events()
{
	
	if (noiseSelector) {
		// which noise is selected
		switch (selectedNoise) {
		case 0: {
			setNoise(randomNoise);
			break;
		}
		case 1: {
			setNoise(valueNoise);
			break;
		}
		case 2: {
			setNoise(perlinNoise);
			break;
		}
		case 3: {
			setNoise(cellularNoise);
			break;
		}
		}
	}
}

void NoiseVisualiser::seedLoop(){
	while (true) {
		// only change seed if not static mode
		if (!isStatic) {
			if (seedAscending && seed < seedMax) {
				seed += 1;
			}
			else {
				seedAscending = false;
			}

			if (!seedAscending && seed > seedMin) {
				seed -= 1;
			}
			else {
				seedAscending = true;
			}

			// delay
			std::this_thread::sleep_for(std::chrono::milliseconds(seedDelayMS));
		}
	}
	
}

void NoiseVisualiser::setNoise(Shader& noise)
{
	if (&noise != activeNoise) {
		activeNoise = &noise;
	}
	seedLoc = GetShaderLocation(*activeNoise, "seed");
	zoomLoc = GetShaderLocation(*activeNoise, "zoom");
	locationLoc = GetShaderLocation(*activeNoise, "location");
	resolutionLoc = GetShaderLocation(*activeNoise, "resolution");

}
