#include "file_generation.hpp"
#include "constants.hpp"
#include "utility.hpp"
#include "assembly_instructions.hpp"
#include <fstream>
#include <iostream>

void OutputFileGenerator::run(std::vector<Line*> lines, CommandLineArguments& clArgs) {
	std::cout << "=== Stage 4: OutputFileGenerator.run() ===" << std::endl;
	
	//	Store lines vector
	this->lines = lines;
	//	1.	Create an output file and open it for writing.

	//	1.1	Determine output file name.
	std::string output_file_name;

	bool binaryFlag = clArgs.flags[BINARY_FLAG_NAME].is_set;

	std::cout << "binary flag is set? " << binaryFlag << std::endl;

	if (clArgs.flags[OUTPUT_FILE_FLAG_NAME].is_set) {
		//	If output file flag is set (-o flag), then set the output file name
		//	to the -o flag value
		output_file_name = clArgs.flags[OUTPUT_FILE_FLAG_NAME].values[0].token_string;
	}
	else {
		//	Otherwise, set the output file name to the input file name
		output_file_name = removeFileExtension(clArgs.input_file_name);

		//	Set output file extension to .bin if binary flag is set and .mlg
		//	otherwise
		output_file_name += (binaryFlag ? ".bin" : ".mlg");
	}

	//	1.2	Open file for writing
	std::ofstream output_file;

	if (binaryFlag) {
		//	Open output file for binary writing
		//output_file.open(output_file_name, std::ofstream::out | std::ofstream::binary);
		output_file.open(output_file_name, std::ios::binary | std::ios::out);
	}
	else {
		//	Open output file for text writing
		output_file.open(output_file_name, std::ofstream::out);
	}
	

	if (!output_file.is_open()) {
		throwError("Could not open file '" + output_file_name 
			+ "' for writing.");
	}

	std::cout << "file is opened for writing\n";

	//	2.	Write to the output file

	if (binaryFlag) {
		this->createBINFile(output_file);
	}
	else {
		this->createMLGFile(output_file);
	}

	//	3.	Close the output file
	output_file.close();

	std::cout << "Output file is closed." << std::endl;
}

void OutputFileGenerator::createBINFile(std::ofstream& output_file) {
	std::cout << "Creating binary file..." << std::endl;
	//	Write to a binary file
	for (Line* line : this->lines) {
		if (line->type == LineType::INSTRUCTION) {
			InstructionLine* instructionLine = (InstructionLine*)line;

			std::cout << "Instruction: " << instructionLine->instruction->name << std::endl;

			std::vector<uint16_t> instruction_code =
				instructionLine->instruction->generateCode(*instructionLine);

			for (uint16_t code : instruction_code) {
				std::cout << "Instruction code: " << std::to_string(code) << std::endl;
				char lowByte = code & 0xff;
				char highByte = (code >> 8) & 0xff;
				output_file.write(&lowByte, 1);
				output_file.write(&highByte, 1);
			}
		}
	}

}

void OutputFileGenerator::createMLGFile(std::ofstream& output_file) {
	//	TODO: fill this
}