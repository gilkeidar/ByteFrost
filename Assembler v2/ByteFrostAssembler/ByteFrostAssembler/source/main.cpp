#include <iostream>
#include "assembler.hpp"

int main(int argc, char** argv) {
	std::cout << "ByteFrost Assembler" << std::endl;
	std::cout << "Time of last compilation: " << __DATE__ << std::endl;
	//	Create Assembler and assemble input file
	Assembler assembler(argc, argv);

	assembler.run();

	return 0;
}