#include <iostream>
#include <string>
#include "assembler.hpp"
#include "utility.hpp"

Assembler::Assembler(int argc, char** argv) : clap(argc, argv, config),
	parser(config), preprocessor(config), label_handler(config), 
	file_generator(config)
{
	//	Initialize Assembler state.
	debug("Assembler constructor.");

	//	Initialize assembly instructions list
	initializeAssemblyInstructions();

	//	Initialize Preprocessor Directives list
	initializePreprocessorDirectives();
}

void Assembler::run() {
	//	Run the Assembler pipeline to produce a machine language file from a
	//	given .asm file.
	debug("Assembler run()");

	//	Stage 0: Command-Line Argument Parsing
	clap.run();

	//	Stage 1: Parser
	parser.run(this->lines, this->instructions, this->directives);

	//	Stage 2: Preprocessor
	preprocessor.run(this->lines);

	//	Stage 3: Label Handler
	label_handler.run(this->lines);

	//	Stage 4: Output File Generation
	file_generator.run(this->lines);
}

void Assembler::initializeAssemblyInstructions() {
	for (const AssemblyInstruction & instruction : assembly_instructions) {
		debug("Adding " + instruction.name + "\tinstruction to assembler...");
		this->instructions[instruction.name].push_back(instruction);
	}
}

void Assembler::initializePreprocessorDirectives() {
	for (const Directive& directive : preprocessor_directives) {
		debug("Adding " + directive.name 
			+ "\tpreprocessor directive to assembler...");
		this->directives[directive.name] = directive;
	}
}