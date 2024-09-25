#pragma once
#include <vector>
#include <string>
#include "constants.hpp"

enum class ArgumentType {
	Rd,
	Rs1,
	Rs2,
	Immediate,
	Func,
	BranchCondition,
	OUTDisplayType,
	SpecialRegister
};

/**
 * @brief Specifies whether an argument value may be in signed (2's complement)
 * representation, unsigned representation, or either.
 */
enum class ArgumentRepresentation {
	Signed,
	Unsigned,
	SignedOrUnsigned
};

std::string ArgumentRepresentationToString(ArgumentRepresentation r);

std::string ArgumentTypeToString(ArgumentType t);

struct ExpectedArgument {
	ArgumentType type;
	ArgumentRepresentation rep;
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

//	Common Argument Definitions
const ExpectedArgument Rd = { ArgumentType::Rd, ArgumentRepresentation::Unsigned, GREGISTER_SIZE, RD_DEF_POSITION };
const ExpectedArgument Rs1 = { ArgumentType::Rs1, ArgumentRepresentation::Unsigned, GREGISTER_SIZE, RS1_DEF_POSITION };
const ExpectedArgument Rs2 = { ArgumentType::Rs2, ArgumentRepresentation::Unsigned, GREGISTER_SIZE, RS2_DEF_POSITION };

const ISAInstruction isa[] = {
	{	//	NOP
		NOP_OPCODE,
		{}
	},
	{	//	BRK
		BRK_OPCODE,
		{}
	},
	{	//	ALU
		ALU_OPCODE,
		{
			{ArgumentType::Func, ArgumentRepresentation::Unsigned, ALU_FUNC_SIZE, 8},
			Rd,
			Rs1,
			Rs2
		}
	},
	{	//	Load Immediate to Reg (LDR)
		LDR_OPCODE,
		{
			Rd,
			{ArgumentType::Immediate, ArgumentRepresentation::SignedOrUnsigned, 8, 8}
		}
	},
	{	//	MOV
		MOV_OPCODE,
		{
			Rd,
			Rs1
		}
	},
	{	//	Branch Absolute Immediate
		BRANCH_ABS_IMM_OPCODE,
		{
			{ArgumentType::BranchCondition, ArgumentRepresentation::Unsigned, BRANCH_COND_SIZE, 5},
			{ArgumentType::Immediate, ArgumentRepresentation::SignedOrUnsigned, 8, 8}
		}
	},
	{	//	ALU Immediate
		ALU_IMM_OPCODE,
		{
			{ArgumentType::Func, ArgumentRepresentation::Unsigned, ALU_FUNC_SIZE, 8},
			Rd,
			{ArgumentType::Immediate, ArgumentRepresentation::Unsigned, 4, 12}
		}
	},
	{	//	Branch Relative Immediate
		BRANCH_REL_IMM_OPCODE,
		{
			{ArgumentType::BranchCondition, ArgumentRepresentation::Unsigned, BRANCH_COND_SIZE, 5},
			{ArgumentType::Immediate, ArgumentRepresentation::Signed, 8, 8}
		}
	},
	{	//	OUT Register
		OUT_REG_OPCODE,
		{
			Rs1,
			{ArgumentType::OUTDisplayType, ArgumentRepresentation::Unsigned, 1, 5}
		}
	},
	{	//	Load Memory Absolute
		LMA_OPCODE,
		{
			Rd,
			{ArgumentType::Immediate, ArgumentRepresentation::Unsigned, 8, 8}
		}
	},
	{	//	Store Memory Absolute
		SMA_OPCODE,
		{
			Rd,
			{ArgumentType::Immediate, ArgumentRepresentation::Unsigned, 8, 8}
		}
	},
	{	//	Load Memory Registers
		LMR_OPCODE,
		{
			Rd,
			Rs1
		}
	},
	{	//	Store Memory Registers
		SMR_OPCODE,
		{
			Rd,
			Rs1
		}
	},
	{	//	OUT Immediate
		OUT_IMM_OPCODE,
		{
			{ArgumentType::Immediate, ArgumentRepresentation::Unsigned, 8, 8},
			{ArgumentType::OUTDisplayType, ArgumentRepresentation::Unsigned, 1, 5}
		}
	},
	{	//	PUSH
		PUSH_OPCODE,
		{
			Rs1
		}
	},
	{	//	POP
		POP_OPCODE,
		{
			Rd
		}
	},
	{	//	JSR
		JSR_OPCODE,
		{
			{ArgumentType::Immediate, ArgumentRepresentation::Unsigned, 8, 8}
		}
	},
	{	//	RTS
		RTS_OPCODE,
		{}
	},
	{	//	Test Reg
		TST_REG_OPCODE,
		{
			{ArgumentType::Func, ArgumentRepresentation::Unsigned, ALU_FUNC_SIZE, 8},
			Rs1,
			Rs2
		}
	},
	{	//	Test Immediate
		TST_IMM_OPCODE,
		{
			{ArgumentType::Func, ArgumentRepresentation::Unsigned, ALU_FUNC_SIZE, 8},
			Rd,
			{ArgumentType::Immediate, ArgumentRepresentation::Unsigned, 4, 12}
		}
	},
	{	//	Load Special Register Immediate
		LSP_IMM_OPCODE,
		{
			{ArgumentType::SpecialRegister, ArgumentRepresentation::Unsigned, SREGISTER_SIZE, 6},
			{ArgumentType::Immediate, ArgumentRepresentation::Unsigned, 8, 8}
		}
	},
};