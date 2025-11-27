#include "assembly_instructions.hpp"
#include "utility.hpp"
#include <sstream>

//	TODO: modify this method so that it also takes a maximum bit size
//	restriction (which comes from the Argument struct in AssemblyArgument)
//	Add a check that if the Token's integer value's minimum bit width is larger
//	than the size, then data loss may (will) occur.
int getTokenIntValue(Token t) {
	//	TODO: If the token's integer value is determined to be negative, throw
	//	a warning since handling negative numbers may not work properly
	//	(e.g., consider ADD R1, #-1 --> since the underlying ISA instruction is
	//	ALU immediate, the immediate that can be stored in the ISA instruction
	//	is 4 bits wide. This means -1 will be represented as 0xF. When the
	//	immediate reaches the ALU register B, however, it is 0 extended, not
	//	sign-extended, meaning it becomes 0x0F, and so the effect is that
	//	R1 = R1 + 15, not the intended R1 = R1 + (-1). Of course, this can be
	//	avoided by using SUB R1, #1, since the ALU will negate the value in
	//	ALU register B after the immediate is 0-extended (so B stores 0x01 and
	//	R1 = R1 - B = R1 + (-B) = R1 + (~B + 1) = R1 + 0xFD + 1 = R1 + 0xFF
	//	   = R1 - 1, as intended))
	// 
	//	ALTERNATIVE: Alternatively, we can add an enum ValueSign or something
	//	like this to the Argument struct that can be SIGNED, UNSIGNED, or
	//	EITHER. If an argument expects an UNSIGNED value, then the immediate
	//	value cannot be negative. I don't think an argument ever expects
	//	something that is explicitly signed, but if so, the value must fit in
	//	the argument size's bit width 2's complement string. If either, then
	//	if the value is nonnegative, it needs to fit in the argument size's bit
	//	width unsigned string; if it's negative, it needs to fit in the
	//	argument size's bit width 2's complement string.
	switch (t.type) {
		case TokenType::GREGISTER:
			//	Return integer value after initial "R", e.g. from "R1" -> 1
			return std::stoi(t.token_string.substr(1));
		case TokenType::OUT_PRINT_TYPE:
			//	Return 1 bit value of OUT_PRINT_TYPE
			if (t.token_string == ASCII_PRINT_TYPE)
				return ASCII_PRINT_TYPE_BIT;
			else if (t.token_string == INTEGER_PRINT_TYPE)
				return INTEGER_PRINT_TYPE_BIT;
			else
				throwError("Unknown "
					+ TokenTypeToString(TokenType::OUT_PRINT_TYPE) 
					+ " '" + t.token_string + "'.");
		case TokenType::SREGISTER: {
			//	TODO: Remove this when LSP is removed.

			//	TODO: Return integer value for each special register
			//throwError("SREGISTER CONVERSION NOT IMPLEMENTED");
			std::string sregister_name = t.token_string.substr(1);
			if (special_register_bits.find(sregister_name) == special_register_bits.end()) {
				throwError("Token string '" + t.token_string + "' is not a valid special register string.");
			}

			return special_register_bits.find(sregister_name)->second;
		}
		case TokenType::AREGISTER: {
			std::string aregister_name = t.token_string.substr(1);
			if (address_register_bits.find(aregister_name) == address_register_bits.end()) {
				throwError("Token string '" + t.token_string + "' is not a valid address register string.");
			}

			return address_register_bits.find(aregister_name)->second;
		}
		case TokenType::IMMEDIATE: {
			//	Return integer value after initial "#", e.g., from "#0x1" -> 1
			//throwWarning("IMMEDIATE CONVERSION MAY NOT WORK PROPERLY");
			//int immediateValue = std::stoi(t.token_string.substr(1));
			int immediateValue = getImmediateValue(t.token_string);
			/*if (immediateValue < 0) {
				throwWarning("Negative immediate value "
					+ std::to_string(immediateValue)
					+ " detected. Negative integer values are not uniformly supported - unintended effects may occur.");
			}*/
			return immediateValue;
		}
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

std::vector<uint16_t> AssemblyInstruction::generateCode(InstructionLine& line,
	Config & config) {
	//	Instruction strings that are equivalent to this AssemblyInstruction
	std::vector<uint16_t> code;

	for (int i = 0; i < this->instruction_sequence.size(); i++) {
		//	Create std::vector<Argument> vector for the current ISAInstruction
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
					//	Verify that the argument value can fit in the Argument's
					//	specified range (based on the Argument's acceptable
					//	representations - signed, unsigned, or either).
					if (!fitsArgumentRange(arg.value, arg.size, this->instruction_sequence[i]->expected_arguments[j].rep)) {
						throwError("Value " + std::to_string(arg.value)
							+ " cannot fit in " 
							+ ArgumentRepresentationToString(this->instruction_sequence[i]->expected_arguments[j].rep)
							+ " as expected by instruction with opcode " 
							+ std::to_string(this->instruction_sequence[i]->opcode)
							+ ".");
					}
					break;
				}
				case ArgumentSource::TokenLabelHigh: {
					//	This argument's value is an index of the token in the
					//	InstructionLine that contains the value. The token
					//	value is an immediate from a Label, and we want the
					//	High byte from it.
					if (assemblyArg.argument.value >= line.tokens.size()) {
						throwError(
							"AssemblyArgument from TokenLabelHigh expects value from token of index "
							+ std::to_string(assemblyArg.argument.value)
							+ ", but only have "
							+ std::to_string(line.tokens.size())
							+ " tokens available in line."
						);
					}
					int label_immediate = getTokenIntValue(line.tokens[assemblyArg.argument.value]);
					arg.value = getByteFromInt(label_immediate, 1);
					//	Verify that the argument value can fit in the Argument's
					//	specified range (based on the Argument's acceptable
					//	representations - signed, unsigned, or either).
					if (!fitsArgumentRange(arg.value, arg.size, this->instruction_sequence[i]->expected_arguments[j].rep)) {
						throwError("Value " + std::to_string(arg.value)
							+ " cannot fit in "
							+ ArgumentRepresentationToString(this->instruction_sequence[i]->expected_arguments[j].rep)
							+ " as expected by instruction with opcode "
							+ std::to_string(this->instruction_sequence[i]->opcode)
							+ ".");
					}
					break;
				}
				case ArgumentSource::TokenLabelLow: {
					//	This argument's value is an index of the token in the
					//	InstructionLine that contains the value. The token
					//	value is an immediate from a Label, and we want the
					//	Low byte from it.
					if (assemblyArg.argument.value >= line.tokens.size()) {
						throwError(
							"AssemblyArgument from TokenLabelLow expects value from token of index "
							+ std::to_string(assemblyArg.argument.value)
							+ ", but only have "
							+ std::to_string(line.tokens.size())
							+ " tokens available in line."
						);
					}
					int label_immediate = getTokenIntValue(line.tokens[assemblyArg.argument.value]);
					arg.value = getByteFromInt(label_immediate, 0);
					//	Verify that the argument value can fit in the Argument's
					//	specified range (based on the Argument's acceptable
					//	representations - signed, unsigned, or either).
					if (!fitsArgumentRange(arg.value, arg.size, this->instruction_sequence[i]->expected_arguments[j].rep)) {
						throwError("Value " + std::to_string(arg.value)
							+ " cannot fit in "
							+ ArgumentRepresentationToString(this->instruction_sequence[i]->expected_arguments[j].rep)
							+ " as expected by instruction with opcode "
							+ std::to_string(this->instruction_sequence[i]->opcode)
							+ ".");
					}
					break;
				}
				case ArgumentSource::CurrentAddressHighOffset: {
					//	This is a special case (currently only used for the
					//	CALL instruction).
					//	The value here is the high byte of 
					//	((this line's address + offset) >> 1), and the offset is 
					//	stored in the argument value.
					//arg.value = logicalShiftRight(getByteFromInt(line.line_address + arg.value, 1), 1);

					//	Updated to shift the line address with the start
					//	address of the program specified in the Assembler's
					//	Config instance
					arg.value = logicalShiftRight(getByteFromInt(line.line_address + arg.value + config.start_address, 1), 1);

					//	Verify that the argument value can fit in the Argument's
					//	specified range (based on the Argument's acceptable
					//	representations - signed, unsigned, or either).
					if (!fitsArgumentRange(arg.value, arg.size, this->instruction_sequence[i]->expected_arguments[j].rep)) {
						throwError("Value " + std::to_string(arg.value)
							+ " cannot fit in "
							+ ArgumentRepresentationToString(this->instruction_sequence[i]->expected_arguments[j].rep)
							+ " as expected by instruction with opcode "
							+ std::to_string(this->instruction_sequence[i]->opcode)
							+ ".");
					}
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