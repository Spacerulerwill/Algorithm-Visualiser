#pragma once
#include "ScreenElement.h"
#include <iostream>
#include <vector>
#include <functional>

class SortingVisualiser : public ScreenElement
{
public:
	//constructor
	SortingVisualiser(float xRatio, float yRatio, float widthRatio, float heightRatio);

	//override draw method
	void draw() override;
	void keyEvents() override;

	//start sort thread
	void startSortThread();

private:

	//state enum class
	enum class State { SORTING, CHECKING, IDLE };
	enum class Sorts { BUBBLE, INSERTION, SELECTION, COUNT };
	enum class Style { STAIRCASE, PRYAMID, CIRCLE};

	//enums
	State state;
	Sorts sort;
	Style style;

	//vectors
	std::vector<int> arr;

	//sorting stats
	std::string title; // title text
	int vectorSize = 1000; //size of vector
	int maxValue = 100; // max value vector can have
	int nsDelay = 0; // delay in nanoseconds
	std::function<void(std::vector<int>&)> sortFunction; // the sorting algorithm function to execute

	int comparisonsMade = 0;
	int swapsMade = 0;
	int arrayAccesses = 0;
	bool colorMode = false;

	//menu bar options
	bool algorithmSelector;
	bool sizeSlider;
	bool maxValSlider;
	bool runButton; 
	bool styleButton;
	bool randomButton;

	int algorithmIndex = 0;

	const char * sortOptions[3] = {"Bubble Sort O(n^2)", "Insertion Sort O(n^2)", "Selection Sort O(n^2)"};
	
	//drawing functions
	void drawBars();
	void drawText();
	void drawSidebar();

	//functions for changing sort and running one and checking sorted
	void setSort(Sorts newSort);
	void executeSort(std::function<void(std::vector<int>&)> sort);
	bool checkArrSorted();

	//sorting functions
	void bubbleSort(std::vector<int>& arr);
	void insertionSort(std::vector<int>& arr);
	void selectionSort(std::vector<int>& arr);
	void countingSort(std::vector<int>& arr);
};

