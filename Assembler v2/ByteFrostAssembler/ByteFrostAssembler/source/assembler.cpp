#include "assembler.hpp"
#include <iostream>
#include <string>

Assembler::Assembler(int argc, char** argv) : clap(argc, argv) {
	//	Initialize Assembler state.
	std::cout << "Assembler constructor." << std::endl;

	//	Initialize assembly instructions list
	initializeAssemblyInstructions();
}

void Assembler::run() {
	//	Run the Assembler pipeline to produce a machine language file from a
	//	given .asm file.
	std::cout << "Assembler run()" << std::endl;

	//	Stage 0: Command-Line Argument Parsing
	this->commandLineArguments = clap.run();

	//	Stage 1: Parser
	this->lines = parser.run(this->commandLineArguments, this->instructions);

	//	Stage 2: Preprocessor

	//	Stage 3: Label Handler

	//	Stage 4: Output File Generation
	file_generator.run(this->lines, *(this->commandLineArguments));
}

void Assembler::initializeAssemblyInstructions() {
	for (const AssemblyInstruction & instruction : assembly_instructions) {
		std::cout << "Adding " << instruction.name 
			+ " instruction to assembler..." << std::endl;
		this->instructions[instruction.name].push_back(instruction);
	}
}