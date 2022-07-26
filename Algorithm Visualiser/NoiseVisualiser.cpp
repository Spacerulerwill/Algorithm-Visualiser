#include "NoiseVisualiser.h"
#include "raylib.h"
#include "imgui.h"
#include <iostream>
#include <iomanip>

NoiseVisualiser::NoiseVisualiser(float xRatio, float yRatio, float widthRatio, float heightRatio) : ScreenElement(xRatio, yRatio, widthRatio, heightRatio)
{
	//initialise random seed
	srand(time(NULL));

	Image imBlank = GenImageColor((int)getWidth(), (int)getHeight(), BLANK);
	noiseTex = LoadTextureFromImage(imBlank);  // Load blank texture to fill on shader
	UnloadImage(imBlank);
}

NoiseVisualiser::~NoiseVisualiser()
{
	UnloadTexture(noiseTex);
}

void NoiseVisualiser::draw()
{
	// draw noise settings
	ImGui::SetNextWindowSize({ 0.0f, 0.0f });
	ImGui::Begin("Noise Settings", NULL, ImGuiWindowFlags_None);

	// noise selector
	noiseSelector = ImGui::Combo("Noise Type", &selectedNoise, noises, numNoises);
	renderNoiseButton = ImGui::Button("Render Noise");
	ImGui::End();

	DrawTexturePro(noiseTex, Rectangle{ getX(), getY(), (float)noiseTex.width, (float)noiseTex.height }, Rectangle{ getX(), getY(), getWidth(), getHeight() }, Vector2{ 0, 0 }, 0.0f, WHITE);
}

void NoiseVisualiser::keyEvents()
{

}

void NoiseVisualiser::renderRandomNoise() {
	//clear image
	Image noiseImg = GenImageColor((int)getWidth(), (int)getHeight(), BLANK);

	
	for (int x = 0; x < getWidth(); x++) {
		for (int y = 0; y < getHeight(); y++) {
			int8_t red = rand() % 0xff + 1;
			int8_t blue = rand() % 0xff + 1;
			int8_t green = rand() % 0xff + 1;

			Color col = {red, blue, green, 255};
			ImageDrawPixel(&noiseImg, x, y, col);
		}
	}
	
	noiseTex = LoadTextureFromImage(noiseImg);
	UnloadImage(noiseImg);
}

void NoiseVisualiser::events()
{
	if (renderNoiseButton) {
		// which noise is selected
		switch (selectedNoise) {
		case 0: {
			renderRandomNoise();
		}
		}
	}
}
