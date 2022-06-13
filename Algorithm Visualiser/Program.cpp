#include "Program.h"
#include "raylib.h"
#include "rlImGui.h"
#include "SortingVisualiser.h"
#include "FractalVisualiser.h"
#include <map>

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

	sortVisualiser = new SortingVisualiser(0.0f, 0.0f, 0.875f, 1.0f);
	fractalVisualiser = new FractalVisualiser(0.0f, 0.0f, 1.0f, 1.0f);
	activeVisualiser = sortVisualiser;

	//execution loop
	while (!WindowShouldClose()) {
		BeginDrawing();
		BeginRLImGui();

		ClearBackground(BLACK);

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
			}
		}

		drawAlgorithmSelector();

		activeVisualiser->update();

		EndRLImGui();
		EndDrawing();
	}

	ShutdownRLImGui();
	ImGui::DestroyContext();
	CloseWindow();

	delete sortVisualiser;
	delete fractalVisualiser;

}

void Program::drawAlgorithmSelector()
{
	ImGui::SetNextWindowPos(ImVec2{GetScreenWidth() - ImGui::GetWindowWidth() - (GetScreenWidth() * (1 / 8.0f)),0}, ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2{ImGui::GetWindowWidth(), 0});

	ImGui::Begin("Algorithm", NULL);

	algorithmTypeSelector = ImGui::Combo("Algorithm Type", &selectedAlgorithmType, algorithmTypes, 5);
	
	ImGui::End();
}
