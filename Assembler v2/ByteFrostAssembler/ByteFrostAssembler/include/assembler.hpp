#pragma once
#include "clap.hpp"
#include "parser.hpp"
#include "assembly_instructions.hpp"

/**
 * @brief Assembler class. Contains all objects that handle software pipeline,
 *	and any "global" state.
 */
class Assembler {
public:
	/**
	 * @brief Assembler constructor that takes in command-line arguments
	 * @param argc The number of command-line arguments.
	 * @param argv Array of char arrays (strings) that contain the command-line
	 *				arguments.
	 */
	Assembler(int argc, char** argv);

	/**
	 * @brief Runs the Assembler to produce an output file with assembled 
	 * machine language.
	 */
	void run();

private:
	//	Global data

	//	Command-Line Arguments
	CommandLineArguments * commandLineArguments;

	//	Line vector
	std::vector<Line*> lines;

	//	Assembly Instruction hashmap 
	//	(string name -> vector<AssemblyInstruction>)
	std::unordered_map<std::string, std::vector<AssemblyInstruction>> instructions;

	/**
	 * @brief Populate Assembly Instruction hashmap.
	 */
	void initializeAssemblyInstructions();

	//	ISA vector (index: opcode -> ISAInstruction)

	//	Pipeline Stage 0: Command-Line Argument Parser (CLAP)
	CLAP clap;

	//	Pipeline Stage 1: Parser
	Parser parser;

	//	...
};