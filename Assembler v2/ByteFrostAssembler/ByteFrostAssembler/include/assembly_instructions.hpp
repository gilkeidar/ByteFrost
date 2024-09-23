#pragma once
#include <string>
#include <vector>
#include "parser.hpp"
#include "isa.hpp"

/**
 * @brief Given a string that represents an immediate, this function returns
 * the immediate's value.
 * @param immediate_string string that represents an immediate.
 * @return integer value of the given immediate string.
 * @note This function throws an error if the given string does not represent
 * an immediate.
 */
int getImmediateValue(std::string immediate_string);

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
int getTokenIntValue(Token t);

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
	{	//	NOP	
		"NOP",
		{},
		{&isa[NOP_OPCODE]},
		{{}}
	},
	{	//	BRK
		"BRK",
		{},
		{&isa[BRK_OPCODE]},
		{{}}
	},
	{	//	OR Rd, Rs1, Rs2
		"OR",
		{TokenType::GREGISTER, TokenType::GREGISTER, TokenType::GREGISTER},
		{&isa[ALU_OPCODE]},
		{
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::Func, ALU_FUNC_SIZE, OR_FUNC_BITS}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs1, GREGISTER_SIZE, SECOND_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs2, GREGISTER_SIZE, THIRD_TOKEN}
				},
			}
		}
	},
	{	//	AND Rd, Rs1, Rs2
		"AND",
		{TokenType::GREGISTER, TokenType::GREGISTER, TokenType::GREGISTER},
		{&isa[ALU_OPCODE]},
		{
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::Func, ALU_FUNC_SIZE, AND_FUNC_BITS}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs1, GREGISTER_SIZE, SECOND_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs2, GREGISTER_SIZE, THIRD_TOKEN}
				},
			}
		}
	},
	{	//	XOR Rd, Rs1, Rs2
		"XOR",
		{TokenType::GREGISTER, TokenType::GREGISTER, TokenType::GREGISTER},
		{&isa[ALU_OPCODE]},
		{
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::Func, ALU_FUNC_SIZE, XOR_FUNC_BITS}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs1, GREGISTER_SIZE, SECOND_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs2, GREGISTER_SIZE, THIRD_TOKEN}
				},
			}
		}
	},
	{	//	NOT Rd, Rs2	(Rs2 since NOT is implemented as ~B, and Rs2 is loaded
		//	into ALU register B)
		"NOT",
		{TokenType::GREGISTER, TokenType::GREGISTER},
		{&isa[ALU_OPCODE]},
		{
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::Func, ALU_FUNC_SIZE, NOT_FUNC_BITS}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs2, GREGISTER_SIZE, SECOND_TOKEN}
				},
			}
		}
	},
	{	//	ADD Rd, Rs1, Rs2
		"ADD",
		{TokenType::GREGISTER, TokenType::GREGISTER, TokenType::GREGISTER},
		{&isa[ALU_OPCODE]},
		{
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::Func, ALU_FUNC_SIZE, ADD_FUNC_BITS}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs1, GREGISTER_SIZE, SECOND_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs2, GREGISTER_SIZE, THIRD_TOKEN}
				},
			}
		}
	},
	{	//	SUB Rd, Rs1, Rs2	(Rd = Rs1 - Rs2 since this does A - B)
		"SUB",
		{TokenType::GREGISTER, TokenType::GREGISTER, TokenType::GREGISTER},
		{&isa[ALU_OPCODE]},
		{
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::Func, ALU_FUNC_SIZE, SUB_FUNC_BITS}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs1, GREGISTER_SIZE, SECOND_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs2, GREGISTER_SIZE, THIRD_TOKEN}
				},
			}
		}
	},
	{	//	ASL Rd, Rs1	(Shift Left A by 1 bit)
		"ASL",
		{TokenType::GREGISTER, TokenType::GREGISTER},
		{&isa[ALU_OPCODE]},
		{
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::Func, ALU_FUNC_SIZE, ASL_FUNC_BITS}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs1, GREGISTER_SIZE, SECOND_TOKEN}
				}
			}
		}
	},
	{	//	ROL Rd, Rs1	(Rotate Left A by 1 bit)
		"ROL",
		{TokenType::GREGISTER, TokenType::GREGISTER},
		{&isa[ALU_OPCODE]},
		{
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::Func, ALU_FUNC_SIZE, ROL_FUNC_BITS}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs1, GREGISTER_SIZE, SECOND_TOKEN}
				},
			}
		}
	},
	{	//	LSR Rd, Rs1	(Logical Shift Right A by 1 bit)
		"LSR",
		{TokenType::GREGISTER, TokenType::GREGISTER},
		{&isa[ALU_OPCODE]},
		{
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::Func, ALU_FUNC_SIZE, LSR_FUNC_BITS}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs1, GREGISTER_SIZE, SECOND_TOKEN}
				},
			}
		}
	},
	{	//	ASR Rd, Rs1	(Arithmetic Shift Right A by 1 bit)
		"ASR",
		{TokenType::GREGISTER, TokenType::GREGISTER},
		{&isa[ALU_OPCODE]},
		{
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::Func, ALU_FUNC_SIZE, ASR_FUNC_BITS}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs1, GREGISTER_SIZE, SECOND_TOKEN}
				}
			}
		}
	},
	{	//	ROR Rd, Rs1	(Rotates A right by 1 bit)
		"ROR",
		{TokenType::GREGISTER, TokenType::GREGISTER},
		{&isa[ALU_OPCODE]},
		{
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::Func, ALU_FUNC_SIZE, ROR_FUNC_BITS}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs1, GREGISTER_SIZE, SECOND_TOKEN}
				},
			}
		}
	},
	{	//	ADC Rd, Rs1, Rs2	(Rd = Rs1 + Rs2 + carry bit)
		"ADC",
		{TokenType::GREGISTER, TokenType::GREGISTER, TokenType::GREGISTER},
		{&isa[ALU_OPCODE]},
		{
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::Func, ALU_FUNC_SIZE, ADC_FUNC_BITS}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs1, GREGISTER_SIZE, SECOND_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs2, GREGISTER_SIZE, THIRD_TOKEN}
				},
			}
		}
	},
	{	//	SBC Rd, Rs1, Rs2	(Rd = Rs1 - Rs2 since this does A - B)
		"SBC",
		{TokenType::GREGISTER, TokenType::GREGISTER, TokenType::GREGISTER},
		{&isa[ALU_OPCODE]},
		{
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::Func, ALU_FUNC_SIZE, SBC_FUNC_BITS}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs1, GREGISTER_SIZE, SECOND_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs2, GREGISTER_SIZE, THIRD_TOKEN}
				},
			}
		}
	},
	{	//	LDR	Rd, #Imm (8-bit)
		"LDR",
		{TokenType::GREGISTER, TokenType::IMMEDIATE},
		{&isa[LDR_OPCODE]},
		{
			{
				{
					ArgumentSource::Token,
					{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Immediate, 8, SECOND_TOKEN}
				},
			}
		}
	},
	{	//	MOV	Rd, Rs1
		"MOV",
		{TokenType::GREGISTER, TokenType::GREGISTER},
		{&isa[MOV_OPCODE]},
		{
			{
				{
					ArgumentSource::Token,
					{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs1, GREGISTER_SIZE, SECOND_TOKEN}
				},
			}
		}
	},
	//{	//	Branch 
	//	"MOV",
	//	{TokenType::GREGISTER, TokenType::GREGISTER},
	//	{&isa[MOV_OPCODE]},
	//	{
	//		{
	//			{
	//				ArgumentSource::Token,
	//				{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN}
	//			},
	//			{
	//				ArgumentSource::Token,
	//				{ArgumentType::Rs1, GREGISTER_SIZE, SECOND_TOKEN}
	//			},
	//		}
	//	}
	//},
};