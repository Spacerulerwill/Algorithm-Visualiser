#pragma once
#include "ScreenElement.h"
#include <chrono>
class NoiseVisualiser : public ScreenElement
{
public:

	NoiseVisualiser(float xRatio, float yRatio, float widthRatio, float heightRatio);
	~NoiseVisualiser();
private:
	// uniforms
	int timeLoc;
	int dimensionLoc;

	// overridable methods
	void draw() override;
	void keyEvents() override;
	void events() override;

	// settings menu
	bool noiseSelector = false;

	bool dimensionSelector = false;
	int selectedDimension = 1;
	const char* dimensions[3] = { "1D", "2D", "3D" };
	bool isStatic = false;

	int selectedNoise = 0;
	const static int numNoises = 1;
	const char* noises[numNoises] = { "Random Noise" };

	Texture2D noiseTex;

	Shader randomNoise;
	Shader* activeNoise;

	void setNoise(Shader& shader);

	std::chrono::steady_clock::time_point start_time;
};

