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
	int seedLoc; 
	int zoomLoc;
	int locationLoc;
	int resolutionLoc;
	int octavesLoc;

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

	int seedMax = 1000;
	int seedMin = 1;
	int seed = seedMin;
	int seedAscending = true;
	int seedDelayMS = 1000;

	float zoom = 10.0f;

	Vector2 location = { 0.0f, 0.0f };
	Vector2 resolution = { getWidth(), getHeight() };

	int octaves = 1;

	// seed function - makes seed fluctuate up and down
	void seedLoop();

	int selectedNoise = 0;
	const static int numNoises = 4;
	const char* noises[numNoises] = { "Random Noise" , "Value Noise", "Perlin Noise", "Cellular Noise"};

	Texture2D noiseTex;

	Shader randomNoise;
	Shader valueNoise;
	Shader perlinNoise;
	Shader cellularNoise;
	Shader* activeNoise;

	void setNoise(Shader& shader);
};

