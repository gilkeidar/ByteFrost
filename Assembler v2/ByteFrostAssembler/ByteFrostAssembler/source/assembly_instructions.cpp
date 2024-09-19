#include "assembly_instructions.hpp"
#include "utility.hpp"

uint16_t getTokenIntValue(Token t) {
	switch (t.type) {
		case TokenType::GREGISTER:
			//	Return integer value after initial "R", e.g. from "R1" -> 1
			return std::stoi(t.token_string.substr(1));
		case TokenType::SREGISTER:
			//	TODO: Return integer value for each special register
			throwError("SREGISTER CONVERSION NOT IMPLEMENTED");
		case TokenType::IMMEDIATE:
			//	Return integer value after initial "#", e.g., from "#0x1" -> 1
			throwWarning("IMMEDIATE CONVERSION MAY NOT WORK PROPERLY");
			return std::stoi(t.token_string.substr(1));
		case TokenType::NUMBER:
		case TokenType::INSTRUCTION:
		case TokenType::TEXT:
		case TokenType::DIRECTIVE:
		case TokenType::LABEL:
		case TokenType::INVALID:
		default:
			throwError("TokenType " + TokenTypeToString(t.type) 
				+ " may not be converted to an integer value.");
	}
}

std::vector<uint16_t> AssemblyInstruction::generateCode(InstructionLine& line) {
	//	INITIAL IMPLEMENTATION - only works for "ADD Rd, Rs1, Rs2"!!!!
	
	//	Instruction strings that are equivalent to this AssemblyInstruction
	std::vector<uint16_t> code;
	
	//	Create std::vector<Argument> vector for each ISAInstruction in the
	//	AssemblyInstruction's instruction sequence
	//	(for ADD Rd, Rs1, Rs2, the instruction sequence is {opcode 2})
	std::vector<Argument> arguments{
		{
			ArgumentType::Func,
			4,
			6
		},
		{
			ArgumentType::Rd,
			2,
			getTokenIntValue(line.tokens[1])	//	Need to convert token value to int value!
		},
		{
			ArgumentType::Rs1,
			2,
			getTokenIntValue(line.tokens[2])
		},
		{
			ArgumentType::Rs2,
			2,
			getTokenIntValue(line.tokens[3])
		}
	};

	return { isa[2].generateCode(arguments) };
}