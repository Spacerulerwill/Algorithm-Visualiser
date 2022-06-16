#include "boost/multiprecision/cpp_dec_float.hpp";
#include <iostream>
#include "Program.h"
#include "raylib.h"

int main() {
	/*
	using namespace boost::multiprecision;

	cpp_dec_float_100 test = sqrt(2);
	std::cout << "Boost Multiprecision: "
		<< std::setprecision(
			std::numeric_limits<cpp_dec_float_100>::digits)
		<< test << std::endl;
		*/

	//create singleton
	Program* program = Program::getInstance();

	return 0;
}