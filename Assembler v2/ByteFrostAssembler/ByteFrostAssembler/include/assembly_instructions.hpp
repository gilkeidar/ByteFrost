#pragma once
#include <string>
#include <vector>
#include "parser.hpp"
#include "isa.hpp"

/**
 * @brief Given a Token t, returns its integer value as it would be represented
 * in an instruction string.
 * @note Only works for Tokens whose TokenType is such that they can be
 * stored in an instruction string. E.g., TEXT, LABEL, and DIRECTIVE tokens
 * cannot be converted to an integer value.
 * @param t Token whose value to convert to an integer as it would be represented
 * in an instruction string.
 * @return integer value of Token t's string value as it would be represented
 * in an instruction string.
 */
uint16_t getTokenIntValue(Token t);

enum class ArgumentSource {Token, Constant};

struct AssemblyArgument {
	ArgumentSource source;
	Argument argument;
};

struct AssemblyInstruction {
	std::string name;
	std::vector<TokenType> expected_param_types;
	std::vector<const ISAInstruction*> instruction_sequence;
	std::vector<std::vector<AssemblyArgument>> instruction_sequence_arguments;
	
	std::vector<uint16_t> generateCode(InstructionLine& line);
};


const AssemblyInstruction assembly_instructions[] = {
	//{"ADD", {TokenType::GREGISTER, TokenType::GREGISTER, TokenType::GREGISTER}},
	//{"ADD", {TokenType::GREGISTER, TokenType::IMMEDIATE}},
	//{"NOP", {}}
	{	//	ADD Rd, Rs1, Rs2
		"ADD",
		{TokenType::GREGISTER, TokenType::GREGISTER, TokenType::GREGISTER},
		{&isa[2]},
		{
			{
				{
					ArgumentSource::Constant,
					{
						ArgumentType::Func,
						4,
						6
					}
				},
				{
					ArgumentSource::Token,
					{
						ArgumentType::Rd,
						2,
						1
					}
				},
				{
					ArgumentSource::Token,
					{
						ArgumentType::Rs1,
						2,
						2
					}
				},
				{
					ArgumentSource::Token,
					{
						ArgumentType::Rs2,
						2,
						3
					}
				},
			}
		}
	}
};