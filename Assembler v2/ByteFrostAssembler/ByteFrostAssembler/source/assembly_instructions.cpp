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
	//	Instruction strings that are equivalent to this AssemblyInstruction
	std::vector<uint16_t> code;

	for (int i = 0; i < this->instruction_sequence.size(); i++) {
		//	Create std::vector<Argument> vector for the current ISAInstrcuction
		//	in the AssemblyInstruction's instruction sequence
		std::vector<Argument> arguments;

		for (int j = 0; j < this->instruction_sequence_arguments[i].size(); j++) {
			AssemblyArgument assemblyArg = this->instruction_sequence_arguments[i][j];

			//	Copy the Argument from the AssemblyArgument
			Argument arg = assemblyArg.argument;

			switch (assemblyArg.source) {
				case ArgumentSource::Token: {
					//	This argument's value is an index of the token in the
					//	InstructionLine that contains the value.
					if (assemblyArg.argument.value >= line.tokens.size()) {
						throwError(
							"AssemblyArgument from Token expects value from token of index "
							+ std::to_string(assemblyArg.argument.value)
							+ ", but only have " 
							+ std::to_string(line.tokens.size())
							+ " tokens available in line."
						);
					}
					arg.value = getTokenIntValue(line.tokens[assemblyArg.argument.value]);
					break;
				}
				case ArgumentSource::Constant: {
					//	No need to do anything; arg value is already correct
					break;
				}
				default:
					break;
			}

			arguments.push_back(arg);
		}
		code.push_back(this->instruction_sequence[i]->generateCode(arguments));
	}

	return code;
}

//std::vector<uint16_t> AssemblyInstruction::generateCode(InstructionLine& line) {
//	//	INITIAL IMPLEMENTATION - only works for "ADD Rd, Rs1, Rs2"!!!!
//	
//	//	Instruction strings that are equivalent to this AssemblyInstruction
//	std::vector<uint16_t> code;
//	
//	//	Create std::vector<Argument> vector for each ISAInstruction in the
//	//	AssemblyInstruction's instruction sequence
//	//	(for ADD Rd, Rs1, Rs2, the instruction sequence is {opcode 2})
//	std::vector<Argument> arguments{
//		{
//			ArgumentType::Func,
//			4,
//			6
//		},
//		{
//			ArgumentType::Rd,
//			2,
//			getTokenIntValue(line.tokens[1])	//	Need to convert token value to int value!
//		},
//		{
//			ArgumentType::Rs1,
//			2,
//			getTokenIntValue(line.tokens[2])
//		},
//		{
//			ArgumentType::Rs2,
//			2,
//			getTokenIntValue(line.tokens[3])
//		}
//	};
//
//	return { isa[2].generateCode(arguments) };
//}