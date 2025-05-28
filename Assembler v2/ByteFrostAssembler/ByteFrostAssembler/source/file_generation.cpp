#include "file_generation.hpp"
#include "constants.hpp"
#include "utility.hpp"
#include "assembly_instructions.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

void OutputFileGenerator::run(std::vector<Line*> & lines) {
	debug("=== Stage 4: OutputFileGenerator.run() ===");
	
	//	Store lines vector
	this->lines = lines;
	//	1.	Create an output file and open it for writing.

	//	1.1	Determine output file name.
	std::string output_file_name;

	//bool binaryFlag = clArgs.flags[BINARY_FLAG_NAME].is_set;

	//debug("binary flag is set? " + binaryFlag);

	debug("binary flag is set? " + config.generateBinaryFile);

	//if (clArgs.flags[OUTPUT_FILE_FLAG_NAME].is_set) {
	if (config.output_file_name != UNSET_FILE_NAME) {
		//	If output file flag is set (-o flag), then set the output file name
		//	to the -o flag value
		//output_file_name = clArgs.flags[OUTPUT_FILE_FLAG_NAME].values[0].token_string;
		output_file_name = config.output_file_name;
	}
	else {
		//	Otherwise, set the output file name to the input file name
		output_file_name = removeFileExtension(config.input_file_name);

		//	Set output file extension to .bin if binary flag is set and .mlg
		//	otherwise
		output_file_name += (config.generateBinaryFile ? ".bin" : ".mlg");
	}

	//	1.2	Open file for writing
	std::ofstream output_file;

	if (config.generateBinaryFile) {
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

	debug("file is opened for writing");

	//	2.	Write to the output file

	if (config.generateBinaryFile) {
		this->createBINFile(output_file);
	}
	else {
		this->createMLGFile(output_file);
	}

	//	3.	Close the output file
	output_file.close();

	debug("Output file is closed.");
}

void OutputFileGenerator::createBINFile(std::ofstream& output_file) {
	debug("Creating binary file...");
	//	Write to a binary file
	for (Line* line : this->lines) {
		if (line->type == LineType::INSTRUCTION) {
			InstructionLine* instructionLine = (InstructionLine*)line;

			debug("Instruction: " + instructionLine->instruction->name);

			std::vector<uint16_t> instruction_code =
				instructionLine->instruction->generateCode(*instructionLine, 
					config);

			for (uint16_t code : instruction_code) {
				char lowByte = code & 0xff;
				char highByte = (code >> 8) & 0xff;

				std::stringstream converter;
				converter << std::hex << (code & 0xff);
				std::string lowByteString = converter.str();
				converter.clear();
				converter << std::hex << ((code >> 8) & 0xff);
				std::string highByteString = converter.str();
				debug("Instruction code: Low: " + lowByteString + " High: " 
					+ highByteString);

				output_file.write(&lowByte, 1);
				output_file.write(&highByte, 1);
			}
		}
	}

}

void OutputFileGenerator::createMLGFile(std::ofstream& output_file) {
	//	TODO: fill this
}