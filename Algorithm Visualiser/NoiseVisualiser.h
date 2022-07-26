#pragma once
#include "ScreenElement.h"
class NoiseVisualiser : public ScreenElement
{
public:

	NoiseVisualiser(float xRatio, float yRatio, float widthRatio, float heightRatio);
	~NoiseVisualiser();
private:
	// overridable methods
	void draw() override;
	void keyEvents() override;
	void events() override;

	// settings menu
	bool noiseSelector = false;
	bool renderNoiseButton = false;

	int selectedNoise = 0;
	const static int numNoises = 1;
	const char* noises[numNoises] = { "Random Noise" };

	Texture2D noiseTex;

	void renderRandomNoise();
};

