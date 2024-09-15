#include "assembler.hpp"
#include <iostream>
#include <string>

Assembler::Assembler(int argc, char** argv) : clap(argc, argv) {
	//	Initialize Assembler state.
	std::cout << "Assembler constructor." << std::endl;
}

void Assembler::run() {
	//	Run the Assembler pipeline to produce a machine language file from a
	//	given .asm file.
	std::cout << "Assembler run()" << std::endl;

	//	Stage 0: Command-Line Argument Parsing
	clap.run();
}