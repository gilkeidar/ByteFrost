#include "file_generation.hpp"
#include "constants.hpp"
#include "utility.hpp"
#include "assembly_instructions.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

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
	debug("Creating MLG file...");

	//	For each line l:
	//	1.	If it is an InstructionLine:
	//		1.	Generate the instruction code as a vector<uint16_t> instruction_code.
	//		1.	For each uint16_t instruction:
	//			1.	If this is the first instruction:
	//				1.	Print the low and high bytes, address, and original line as:
	//					"0xLB,\t0xHB,\t// 0x[ADDRESS]: [ORIGINAL LINE]"
	//			2.	Otherwise:
	//				1.	Print the low and high bytes and address as:
	//					"0xLB,\t0xHB,\t// 0x[ADDRESS]: ([INSTRUCTION NAME] continued)
	//	2.	Otherwise:
	//		1.	Print the original line string.
	for (Line* line : this->lines) {
		if (line->type == LineType::INSTRUCTION) {
			InstructionLine* instructionLine = (InstructionLine*)line;
			
			std::vector<uint16_t> instruction_code =
				instructionLine->instruction->generateCode(*instructionLine, config);

			//for (uint16_t code : instruction_code) {
			for (size_t i = 0; i < instruction_code.size(); i++) {
				uint16_t code = instruction_code[i];
				std::stringstream converter;
				converter << "0x" << std::hex << std::setw(2) << std::setfill('0') << (code & 0xff)
					<< ", 0x" << std::hex << std::setw(2) << std::setfill('0') << ((code >> 8) & 0xff)
					<< ",\t// 0x" << std::hex << std::setw(4) << std::setfill('0') <<  (instructionLine->line_address + config.start_address + (i << 1)) << ": ";
				if (i == 0) {
					converter << instructionLine->original_string;
				}
				else {
					converter << "(" << instructionLine->instruction->name << " continued)";
				}
				converter << "\n";

				std::string instructionString = converter.str();
				converter.clear();
				output_file.write(instructionString.c_str(), instructionString.length());
			}
		}
		else {
			//	Write original line as is
			std::string lineString = line->original_string + "\n";
			output_file.write(lineString.c_str(), lineString.length());
		}
	}
}