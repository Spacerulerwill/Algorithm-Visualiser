#include "NoiseVisualiser.h"
#include "imgui.h"
#include <iostream>
#include <chrono>

#define GLSL_VERSION 450

NoiseVisualiser::NoiseVisualiser(float xRatio, float yRatio, float widthRatio, float heightRatio) : ScreenElement(xRatio, yRatio, widthRatio, heightRatio)
{
	start_time = std::chrono::steady_clock::now();

	//initialise random seed
	srand(time(NULL));

	randomNoise = LoadShader(0, TextFormat("shaders/noise/randomnoise.fs", GLSL_VERSION));

	Image imBlank = GenImageColor((int)getWidth(), (int)getHeight(), BLANK);
	noiseTex = LoadTextureFromImage(imBlank);  // Load blank texture to fill on shader
	UnloadImage(imBlank);

	setNoise(randomNoise);

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
	ImGui::Begin("Noise Settings", NULL, ImGuiWindowFlags_None);

	// noise selector
	noiseSelector = ImGui::Combo("Noise Type", &selectedNoise, noises, numNoises);
	ImGui::End();

	BeginShaderMode(*activeNoise);
	DrawTexturePro(noiseTex, Rectangle{ getX(), getY(), (float)noiseTex.width, (float)noiseTex.height }, Rectangle{ getX(), getY(), getWidth(), getHeight() }, Vector2{ 0, 0 }, 0.0f, WHITE);
	EndShaderMode();

	std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now();

	int time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();

	SetShaderValue(*activeNoise, timeLoc, &time, SHADER_UNIFORM_INT);
}

void NoiseVisualiser::keyEvents()
{

}

void NoiseVisualiser::events()
{
	if (noiseSelector) {
		// which noise is selected
		switch (selectedNoise) {
		case 0: {
			setNoise(randomNoise);
		}
		}
	}
}

void NoiseVisualiser::setNoise(Shader& noise)
{
	if (&noise != activeNoise) {
		activeNoise = &noise;
	}
	timeLoc = GetShaderLocation(*activeNoise, "time");
}
