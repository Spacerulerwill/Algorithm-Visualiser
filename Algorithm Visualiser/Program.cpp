#include "Program.h"
#include "raylib.h"
#include "rlImGui.h"
#include "SortingVisualiser.h"
#include "FractalVisualiser.h"
#include "NoiseVisualiser.h"
#include <thread>

//singleton stuff
Program* Program::Instance = nullptr;

Program* Program::getInstance()
{
	if (Instance == nullptr) {
		Instance = new Program();
	}
	return Instance;
}

Program::Program()
{
	// Window Configuration
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Algorithm Visualiser");
	SetupRLImGui(true);

	// FPS
	SetTargetFPS(FPS);

	//initialise visualiser modules
	sortVisualiser = new SortingVisualiser(0.0f, 0.0f, 0.875f, 1.0f);
	fractalVisualiser = new FractalVisualiser(0.0f, 0.0f, 1.0f, 1.0f);
	noiseVisualiser = new NoiseVisualiser(0.0f, 0.0f, 1.0f, 1.0f);

	//set active visualiser to sort visualiser
	activeVisualiser = sortVisualiser;

	//execution loop
	while (!WindowShouldClose()) {
		BeginDrawing();
		BeginRLImGui();

		ClearBackground(BLACK); // clear background
		
		//switching algorithm type logic
		if (algorithmTypeSelector) {
			switch (selectedAlgorithmType) {
			case 0: {
				activeVisualiser = sortVisualiser;
				break;
			}
			case 1: {
				activeVisualiser = fractalVisualiser;
				break;
			}
			case 2: {
				activeVisualiser = noiseVisualiser;
				break;
			}
			}
		}

		drawAlgorithmSelector(); // draw algorithm selector ImGui widget
		activeVisualiser->update(); // update active visualiser

		EndRLImGui();
		EndDrawing();
	}

	// closing raylib and ImGui
	ShutdownRLImGui();
	ImGui::DestroyContext();
	CloseWindow();

	// delete visualisers to avoid memory leak
	delete sortVisualiser;
	delete fractalVisualiser;
	delete noiseVisualiser;
}

void Program::drawAlgorithmSelector()
{
	//create algorithm selector, size it and place it
	ImGui::SetNextWindowPos(ImVec2{GetScreenWidth() - ImGui::GetWindowWidth() - (GetScreenWidth() * (1 / 8.0f)),0}, ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2{ImGui::GetWindowWidth(), 0});

	ImGui::Begin("Algorithm", NULL);

	//drop down menu for all the algorithm types
	algorithmTypeSelector = ImGui::Combo("Algorithm Type", &selectedAlgorithmType, algorithmTypes, 5);
	
	ImGui::End();
}
