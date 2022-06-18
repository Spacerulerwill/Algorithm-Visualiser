#include <iostream>
#include "Program.h"
#include "raylib.h"

int main() {
	//create singleton
	Program* program = Program::getInstance();

	// program successfully executed
	return 0;
}