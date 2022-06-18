#include "SortingVisualiser.h"
#include "imgui.h"
#include "imgui_impl_raylib.h"
#include "imgui_internal.h"
#include "rlImGui.h"
#include <fstream>
#include <thread>
#include <random>
#include <sstream>
#include <vector>

// LOGIC
SortingVisualiser::SortingVisualiser(float xRatio, float yRatio, float widthRatio, float heightRatio) : ScreenElement(xRatio, yRatio, widthRatio, heightRatio)
{
	//initialise random seed
	srand(time(NULL));

	// initialise random vector
	for (int i = 0; i < vectorSize; i++)
		arr.push_back(rand() % maxValue + 1);

	//set to not sorting, staircase mode, and activate bubble sort
	state = State::IDLE;
	style = Style::STAIRCASE;
	setSort(Sorts::BUBBLE);
}

//changing sort algorithm
void SortingVisualiser::setSort(Sorts newSort) {
	std::ifstream ifs;
	sort = newSort;
	switch (sort) {
	case Sorts::BUBBLE: {
		title = "Bubble Sort - O(n^2)";
		sortFunction = std::bind(&SortingVisualiser::bubbleSort, this, std::placeholders::_1);
		ifs = std::ifstream("info/bubblesort.txt"); // read info from file
		break;
	}
	case Sorts::INSERTION: {
		title = "Insertion Sort - O(n^2)";
		sortFunction = std::bind(&SortingVisualiser::insertionSort, this, std::placeholders::_1);
		ifs = std::ifstream("info/insertionsort.txt"); // read info from file
		break;
	}
	case Sorts::SELECTION: {
		title = "Selection Sort - O(n^2)";
		sortFunction = std::bind(&SortingVisualiser::selectionSort, this, std::placeholders::_1);
		ifs = std::ifstream("info/selectionsort.txt"); // read info from file
		break;
	}
	case Sorts::COUNT: {
		title = "Counting Sort - O(n+k)";
		sortFunction = std::bind(&SortingVisualiser::countingSort, this, std::placeholders::_1);
		ifs = std::ifstream("info/countingsort.txt"); // read info from file
		break;
	}
	}
	//set new informationt text
	informationText = std::string((std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>()));
}

//start sort
void SortingVisualiser::executeSort(std::function<void(std::vector<int>&)> sort)
{
	state = State::SORTING;
	comparisonsMade = 0;
	swapsMade = 0;
	arrayAccesses = 0;

	// start a thread;
	sort(arr);

	//set idle
	state = State::IDLE;

	//go through once more and check it again. Highlight them green as you to check if sorted
	state = State::CHECKING;
	checkArrSorted();

	//wait a second then, go back to normal
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	state = State::IDLE;
}

// start the sort thread execution
void SortingVisualiser::startSortThread() {
	if (state == State::IDLE) {
		std::thread th(&SortingVisualiser::executeSort, this, sortFunction);
		th.detach();
	}
}

//check array is sorted
bool SortingVisualiser::checkArrSorted() {

	// Array has one or no element
	if (arr.size() == 0 || arr.size() == 1)
		return true;

	for (int i = 1; i < arr.size(); i++) {
		// Unsorted pair found
		if (arr[i - 1] > arr[i]) {
			return false;
		}

		//std::this_thread::sleep_for(std::chrono::nanoseconds((int)(1000000000.0f/vectorSize)));
	}

	// No unsorted pair found
	return true;
}

// DRAWING

//call draw functions
void SortingVisualiser::draw() {
	drawBars();
	drawText();
	drawSidebar();
}

void SortingVisualiser::drawSidebar() {
	ImGui::SetNextWindowPos(ImVec2{ GetScreenWidth() - (GetScreenWidth() / 8.0f),0.0f });
	ImGui::SetNextWindowSize(ImVec2{ GetScreenWidth() / 8.0f,(float)GetScreenHeight() });

	ImGui::Begin("Control Panel", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

	ImVec2 vMin = ImGui::GetWindowContentRegionMin();
	ImVec2 vMax = ImGui::GetWindowContentRegionMax();

	float width = vMax.x - vMin.x;
	float height = vMax.y - vMin.y;

	ImGui::BeginChild("Algorithm", ImVec2{width, height * (1.5f/20.0f)}, true, ImGuiWindowFlags_ChildWindow);
	
	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !(state == State::IDLE)); // disable controls if sorting
	ImGui::Text("Select Algorithm");
	ImGui::PushItemWidth(ImGui::GetWindowWidth() - ImGui::GetStyle().FramePadding.x * 4);
	algorithmSelector = ImGui::Combo("##", &algorithmIndex, sortOptions, 4);
	ImGui::PopItemFlag();

	ImGui::EndChild();

	ImGui::BeginChild("Settings", ImVec2{ width,height * (1 / 3.0f*(18.5f/20.0f)) }, true, ImGuiWindowFlags_ChildWindow);

	ImGui::Text("Settings/Controls");

	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !(state == State::IDLE)); // disable controls if sorting
	sizeSlider = ImGui::SliderInt("Size",  &vectorSize, 5, 1000, " % .3f", 1.0f);
	ImGui::PopItemFlag();

	ImGui::SliderInt("Delay", &nsDelay, 0, 5000000, "% .3f", 1.0f);

	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !(state == State::IDLE)); // disable controls if sorting
	maxValSlider = ImGui::SliderInt("Max", &maxValue, 1, 100, "% .3f", 1.0f);
	ImGui::PopItemFlag();

	//run button
	if (state == State::IDLE) {
		runButton = ImGui::Button("Run");
	}
	else {
		runButton = ImGui::Button("Cancel"); 
	}

	ImGui::SameLine();

	styleButton = ImGui::Button("Style"); 

	ImGui::SameLine();

	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !(state == State::IDLE)); // disable controls if sorting
	randomButton = ImGui::Button("Shuffle");
	ImGui::PopItemFlag();

	ImGui::EndChild();

	ImGui::BeginChild("Information", ImVec2{ width,height * (2 / 3.0f) * (18.5f/20.0f)}, true, ImGuiWindowFlags_ChildWindow);

	ImGui::Text("Information");

	ImGui::TextWrapped(informationText.c_str());

	ImGui::EndChild();

	ImGui::End();
}

//draw bars
void SortingVisualiser::drawBars()
{
	switch (style) {
	case Style::STAIRCASE:
	{
		float BarWidth = getWidth() / vectorSize;

		for (int i = 0; i < vectorSize; i++) {
			//white by default
			Color color;

			if (i == indexAccessing) {
				color = RED;
			}
			else {
				color = WHITE;
			}
			//height of bar relative to screen height
			float height = (arr[i] / (float)maxValue) * (getHeight() * 0.75);

			//height to move down (0,0 is at top raylib)
			float vertDiff = getHeight() - height;

			//draw
			DrawRectangleV(Vector2{ (float)i * BarWidth + getX(),vertDiff + getY() }, Vector2{ BarWidth,height }, color);
		}
		break;
	}
	case Style::PRYAMID:
	{
		float barHeight = getHeight() / vectorSize;

		for (int i = 0; i < vectorSize; i++) {
			Color color;

			if (i == indexAccessing) {
				color = RED;
			}
			else {
				color = WHITE;
			}

			//height of bar relative to screen height
			float width = (arr[i] / (float)maxValue) * (getWidth() * 0.75);

			//amount to shift across
			float horzDiff = (getWidth() - width) / 2;

			//draw
			DrawRectangleV(Vector2{ getX() + horzDiff,(float)i * barHeight + getY() }, Vector2{ width,barHeight }, color);

		}
		break;
	}
	case Style::CIRCLE:
	{
		Vector2 center = { getX() + getWidth() / 2.0f , getY() + getHeight() / 2.0f };
		float radius = getWidth() / 4.0f;
			
		float angle = 360.0f / vectorSize;

		for (int i = 0; i < vectorSize; i++) {
			Color color;

			if (i == indexAccessing) {
				color = BLACK;
			}
			else {
				color = ColorFromHSV(arr[i] * (360.0f / maxValue), 1.0f, 1.0f);
			}
			DrawCircleSector(center, radius, i * angle, (i +1) * angle, 0, color);
		}
		break;
		
	}
	}
	
}

//draw text
void SortingVisualiser::drawText()
{
	// sort info text
	float font = (GetScreenWidth()) / 50;
	std::stringstream stream;
	stream << title << "\n";
	stream << nsDelay / 1000000.0 << " ms delay\n";
	stream << comparisonsMade << " comparisons made\n";
	stream << swapsMade << " swaps made\n";
	stream << arrayAccesses << " array accesses";
	std::string s = stream.str();
	const char* text = s.c_str();
	DrawText(text, 0, 0, font, WHITE);
}

void SortingVisualiser::keyEvents() {
	if (state == State::IDLE) {
		if (IsKeyPressed(KEY_SPACE) || randomButton)
			std::shuffle(std::begin(arr), std::end(arr), std::default_random_engine());
	}

	if (runButton) {
		if (state == State::IDLE) {
			startSortThread();
		}
	}

	if (styleButton) {
		int currentStyle = static_cast<int>(style);
		if (currentStyle == 2)
			currentStyle = -1;

		style = static_cast<Style>(currentStyle + 1);
	}

	//resizing array
	if (sizeSlider || maxValSlider) {
		arr.clear();
		for (int i = 0; i < vectorSize; i++)
			arr.push_back(rand() % maxValue + 1);
	}

	if (algorithmSelector) {
		setSort((Sorts)algorithmIndex);
	}
}

// SORTING ALGORITHMS

void SortingVisualiser::bubbleSort(std::vector<int>& arr)
{
	//index accessing array filled with max amount accessed at any given time
	int i, j;
	for (i = 0; i < arr.size() - 1; i++) {
		for (j = 0; j < arr.size() - i - 1; j++) {
			indexAccessing = j;
			std::this_thread::sleep_for(std::chrono::nanoseconds(nsDelay));
			comparisonsMade++;
			arrayAccesses += 2;
			if (arr[j] > arr[j + 1]) {
				arrayAccesses += 2;
				std::this_thread::sleep_for(std::chrono::nanoseconds(nsDelay));
				std::swap(arr[j], arr[j + 1]);
				swapsMade++;
			}
		}
		if (swapsMade == 0) {
			break;
		}
	}
	indexAccessing = -1;
}

void SortingVisualiser::insertionSort(std::vector<int>& arr)
{

	int i, key, j;
	for (i = 1; i < arr.size(); i++)
	{
		comparisonsMade++;
		key = arr[i];
		arrayAccesses++;
		j = i - 1;
		std::this_thread::sleep_for(std::chrono::nanoseconds(nsDelay));

		while (j >= 0 && arr[j] > key)
		{
			indexAccessing = j;
			comparisonsMade++;
			arrayAccesses += 2;

			std::this_thread::sleep_for(std::chrono::nanoseconds(nsDelay));
			swapsMade++;
			arr[j + 1] = arr[j];
			j = j - 1;
		}
		arr[j + 1] = key;
		arrayAccesses++;
	}
	indexAccessing = -1;
}

void SortingVisualiser::selectionSort(std::vector<int>& arr)
{
	for (int step = 0; step < arr.size() - 1; step++) {
		int min_idx = step;
		for (int i = step + 1; i < arr.size(); i++) {
			indexAccessing = i;
			std::this_thread::sleep_for(std::chrono::nanoseconds(nsDelay));
			comparisonsMade++;
			arrayAccesses += 2;
			if (arr[i] < arr[min_idx]) {
				min_idx = i;
				std::this_thread::sleep_for(std::chrono::nanoseconds(nsDelay));

			}
		}
		// put min at the correct position
		std::swap(arr[min_idx], arr[step]);
		swapsMade++;
	}
	indexAccessing = -1;
}

void SortingVisualiser::countingSort(std::vector<int>& arr)
{
	//first pass, determine minimum and maximum elements
	int min = arr[0];
	int max = arr[0];
	for (int i = 0; i < arr.size(); i++) {
		indexAccessing = i;
		if (arr[i] > max) max = arr[i];
		if (arr[i] < min) min = arr[i];
		std::this_thread::sleep_for(std::chrono::nanoseconds(nsDelay));
	}

	//create count array of size of max-min
	std::vector<int> count(max-min+1,0);
	std::cout << count[0];

	//iterate through once again, incrementing elements in count array
	for (int i = 0; i < arr.size(); i++) {
		indexAccessing = i;
		count[arr[i] - min] += 1;				
		std::this_thread::sleep_for(std::chrono::nanoseconds(nsDelay));
	}

	//construct new vector
	std::vector<int> newArr;

	//reconstruct
	for (int value = 0; value < count.size(); value++) {
		for (int j = 0; j < count[value]; j++) {
			newArr.push_back(value);
		}
	}

	//replace old arr with new arr
	for (int i = 0; i < arr.size(); i++) {
		arr[i] = newArr[i];
		indexAccessing = i;
		std::this_thread::sleep_for(std::chrono::nanoseconds(nsDelay));
	}
	indexAccessing = -1;
}