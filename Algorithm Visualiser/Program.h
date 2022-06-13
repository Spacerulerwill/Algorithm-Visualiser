#pragma once

#include "SortingVisualiser.h"
#include "FractalVisualiser.h"

class Program
{
public:
	static Program* getInstance();

	SortingVisualiser* sortVisualiser;
	ScreenElement* activeVisualiser;
	ScreenElement* fractalVisualiser;

private:

	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 720;
	const int FPS = 120;

	static Program* Instance;

	Program();
	void drawAlgorithmSelector();

	const char* algorithmTypes[5] = {"Sorting", "Fractal", "Noise", "Maze Gen", "Pathfinding"};
	int selectedAlgorithmType = 0;
	bool algorithmTypeSelector = false;

};

