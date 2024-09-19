#pragma once
#include <vector>
#include <string>

enum class ArgumentType {
	Rd,
	Rs1,
	Rs2,
	Immediate,
	Func
};

std::string ArgumentTypeToString(ArgumentType t);

struct ExpectedArgument {
	ArgumentType type;
	int size;		//	size of argument in bits
	int position;	//	position of argument's starting bit in instruction string
};

struct Argument {
	ArgumentType type;
	int size;
	int value;
};

struct ISAInstruction {
	int opcode;
	std::vector<ExpectedArgument> expected_arguments;

	uint16_t generateCode(std::vector<Argument> arguments) const;
};

const ISAInstruction isa[] = {
	{	//	NOP
		0,
		{}
	},
	{	//	BRK
		1,
		{}
	},
	{
		//	ALU
		2,
		{
			{ArgumentType::Func, 4, 8},
			{ArgumentType::Rd, 2, 6},
			{ArgumentType::Rs1, 2, 12},
			{ArgumentType::Rs2, 2, 14}
		}
	}
};