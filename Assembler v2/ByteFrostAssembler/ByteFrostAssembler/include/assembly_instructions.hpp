#pragma once
#include <string>
#include <vector>
//#include "parser.hpp"
#include "shared_types.hpp"
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
int getTokenIntValue(Token t);

enum class ArgumentSource {Token, Constant, TokenLabelHigh, TokenLabelLow,
	CurrentAddressHighOffset};

struct AssemblyArgument {
	ArgumentSource source;
	Argument argument;
};

struct AssemblyInstruction {
	std::string name;
	std::vector<TokenType> expected_param_types;
	std::vector<const ISAInstruction*> instruction_sequence;
	std::vector<std::vector<AssemblyArgument>> instruction_sequence_arguments;
	
	std::vector<uint16_t> generateCode(InstructionLine& line, Config & config);
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
				},
				{	//	Argument ignored by ASL
					ArgumentSource::Constant,
					{ArgumentType::Rs2, GREGISTER_SIZE, 0}
				},
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
				{	//	Argument ignored by ROL
					ArgumentSource::Constant,
					{ArgumentType::Rs2, GREGISTER_SIZE, 0}
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
				{	//	Argument ignored by LSR
					ArgumentSource::Constant,
					{ArgumentType::Rs2, GREGISTER_SIZE, 0}
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
				},
				{	//	Argument ignored by ASR
					ArgumentSource::Constant,
					{ArgumentType::Rs2, GREGISTER_SIZE, 0}
				},
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
				{	//	Argument ignored by ROR
					ArgumentSource::Constant,
					{ArgumentType::Rs2, GREGISTER_SIZE, 0}
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
	{	//	JMP :label
		//	TODO: REMOVE THIS IMPLEMENTATION OF JMP REPLACING LSP WITH LDA OR
		//	ANOTHER INSTRUCTION WHEN LSP IS REMOVED.
		"JMP",
		{TokenType::LABEL},
		{&isa[LDA_OPCODE], &isa[BRANCH_ABS_IMM_OPCODE]},
		{
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::ARDest, AREGISTER_SIZE, PC_BITS}
				},
				{
					ArgumentSource::Constant,
					{ArgumentType::ARHorL, 1, ARHorL_HIGH_BITS}
				},
				{
					ArgumentSource::TokenLabelHigh,
					{ArgumentType::Immediate, 8, FIRST_TOKEN}
				}
			},
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::BranchCondition, BRANCH_COND_SIZE, JMP_COND_BITS}
				},
				{
					ArgumentSource::TokenLabelLow,
					{ArgumentType::Immediate, 8, FIRST_TOKEN}
				},
			}
		}
	},
	{	//	BMI :label
		//	TODO: REMOVE THIS IMPLEMENTATION OF BMI REPLACING LSP WITH LDA OR
		//	ANOTHER INSTRUCTION WHEN LSP IS REMOVED.
		"BMI",
		{TokenType::LABEL},
		{&isa[LDA_OPCODE], &isa[BRANCH_ABS_IMM_OPCODE]},
		{
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::ARDest, AREGISTER_SIZE, PC_BITS}
				},
				{
					ArgumentSource::Constant,
					{ArgumentType::ARHorL, 1, ARHorL_HIGH_BITS}
				},
				{
					ArgumentSource::TokenLabelHigh,
					{ArgumentType::Immediate, 8, FIRST_TOKEN}
				}
			},
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::BranchCondition, BRANCH_COND_SIZE, BMI_COND_BITS}
				},
				{
					ArgumentSource::TokenLabelLow,
					{ArgumentType::Immediate, 8, FIRST_TOKEN}
				},
			}
		}
	},
	{	//	BCS :label
		//	TODO: REMOVE THIS IMPLEMENTATION OF BCS REPLACING LSP WITH LDA OR
		//	ANOTHER INSTRUCTION WHEN LSP IS REMOVED.
		"BCS",
		{TokenType::LABEL},
		{&isa[LDA_OPCODE], &isa[BRANCH_ABS_IMM_OPCODE]},
		{
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::ARDest, AREGISTER_SIZE, PC_BITS}
				},
				{
					ArgumentSource::Constant,
					{ArgumentType::ARHorL, 1, ARHorL_HIGH_BITS}
				},
				{
					ArgumentSource::TokenLabelHigh,
					{ArgumentType::Immediate, 8, FIRST_TOKEN}
				}
			},
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::BranchCondition, BRANCH_COND_SIZE, BCS_COND_BITS}
				},
				{
					ArgumentSource::TokenLabelLow,
					{ArgumentType::Immediate, 8, FIRST_TOKEN}
				},
			}
		}
	},
	{	//	BEQ :label
		//	TODO: REMOVE THIS IMPLEMENTATION OF BEQ REPLACING LSP WITH LDA OR
		//	ANOTHER INSTRUCTION WHEN LSP IS REMOVED.
		"BEQ",
		{TokenType::LABEL},
		{&isa[LDA_OPCODE], &isa[BRANCH_ABS_IMM_OPCODE]},
		{
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::ARDest, AREGISTER_SIZE, PC_BITS}
				},
				{
					ArgumentSource::Constant,
					{ArgumentType::ARHorL, 1, ARHorL_HIGH_BITS}
				},
				{
					ArgumentSource::TokenLabelHigh,
					{ArgumentType::Immediate, 8, FIRST_TOKEN}
				}
			},
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::BranchCondition, BRANCH_COND_SIZE, BEQ_COND_BITS}
				},
				{
					ArgumentSource::TokenLabelLow,
					{ArgumentType::Immediate, 8, FIRST_TOKEN}
				},
			}
		}
	},
	{	//	BPL :label
		//	TODO: REMOVE THIS IMPLEMENTATION OF BPL REPLACING LSP WITH LDA OR
		//	ANOTHER INSTRUCTION WHEN LSP IS REMOVED.
		"BPL",
		{TokenType::LABEL},
		{&isa[LDA_OPCODE], &isa[BRANCH_ABS_IMM_OPCODE]},
		{
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::ARDest, AREGISTER_SIZE, PC_BITS}
				},
				{
					ArgumentSource::Constant,
					{ArgumentType::ARHorL, 1, ARHorL_HIGH_BITS}
				},
				{
					ArgumentSource::TokenLabelHigh,
					{ArgumentType::Immediate, 8, FIRST_TOKEN}
				}
			},
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::BranchCondition, BRANCH_COND_SIZE, BPL_COND_BITS}
				},
				{
					ArgumentSource::TokenLabelLow,
					{ArgumentType::Immediate, 8, FIRST_TOKEN}
				},
			}
		}
	},
	{	//	BCC :label
		//	TODO: REMOVE THIS IMPLEMENTATION OF BCC REPLACING LSP WITH LDA OR
		//	ANOTHER INSTRUCTION WHEN LSP IS REMOVED.
		"BCC",
		{TokenType::LABEL},
		{&isa[LDA_OPCODE], &isa[BRANCH_ABS_IMM_OPCODE]},
		{
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::ARDest, AREGISTER_SIZE, PC_BITS}
				},
				{
					ArgumentSource::Constant,
					{ArgumentType::ARHorL, 1, ARHorL_HIGH_BITS}
				},
				{
					ArgumentSource::TokenLabelHigh,
					{ArgumentType::Immediate, 8, FIRST_TOKEN}
				}
			},
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::BranchCondition, BRANCH_COND_SIZE, BCC_COND_BITS}
				},
				{
					ArgumentSource::TokenLabelLow,
					{ArgumentType::Immediate, 8, FIRST_TOKEN}
				},
			}
		}
	},
	{	//	BNE :label
		//	TODO: REMOVE THIS IMPLEMENTATION OF BNE REPLACING LSP WITH LDA OR
		//	ANOTHER INSTRUCTION WHEN LSP IS REMOVED.
		"BNE",
		{TokenType::LABEL},
		{&isa[LDA_OPCODE], &isa[BRANCH_ABS_IMM_OPCODE]},
		{
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::ARDest, AREGISTER_SIZE, PC_BITS}
				},
				{
					ArgumentSource::Constant,
					{ArgumentType::ARHorL, 1, ARHorL_HIGH_BITS}
				},
				{
					ArgumentSource::TokenLabelHigh,
					{ArgumentType::Immediate, 8, FIRST_TOKEN}
				}
			},
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::BranchCondition, BRANCH_COND_SIZE, BNE_COND_BITS}
				},
				{
					ArgumentSource::TokenLabelLow,
					{ArgumentType::Immediate, 8, FIRST_TOKEN}
				},
			}
		}
	},
	{	//	OR Rd, Immediate
		"OR",
		{TokenType::GREGISTER, TokenType::IMMEDIATE},
		{&isa[ALU_IMM_OPCODE]},
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
					{ArgumentType::Immediate, 4, SECOND_TOKEN}
				},
			}
		}
	},
	{	//	AND Rd, Immediate
		"AND",
		{TokenType::GREGISTER, TokenType::IMMEDIATE},
		{&isa[ALU_IMM_OPCODE]},
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
					{ArgumentType::Immediate, 4, SECOND_TOKEN}
				},
			}
		}
	},
	{	//	XOR Rd, Immediate
		"XOR",
		{TokenType::GREGISTER, TokenType::IMMEDIATE},
		{&isa[ALU_IMM_OPCODE]},
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
					{ArgumentType::Immediate, 4, SECOND_TOKEN}
				},
			}
		}
	},
	{	//	NOT Rd, Immediate
		"NOT",
		{TokenType::GREGISTER, TokenType::IMMEDIATE},
		{&isa[ALU_IMM_OPCODE]},
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
					{ArgumentType::Immediate, 4, SECOND_TOKEN}
				},
			}
		}
	},
	{	//	ADD Rd, Immediate
		"ADD",
		{TokenType::GREGISTER, TokenType::IMMEDIATE},
		{&isa[ALU_IMM_OPCODE]},
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
					{ArgumentType::Immediate, 4, SECOND_TOKEN}
				},
			}
		}
	},
	{	//	SUB Rd, Immediate
		"SUB",
		{TokenType::GREGISTER, TokenType::IMMEDIATE},
		{&isa[ALU_IMM_OPCODE]},
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
					{ArgumentType::Immediate, 4, SECOND_TOKEN}
				},
			}
		}
	},
	{	//	ASL Rd	(Rd = Rd << 1 (arithmetic shift left of Rd) )
		"ASL",
		{TokenType::GREGISTER},
		{&isa[ALU_IMM_OPCODE]},
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
				{	//	This immediate is ignored by ASL (shifts left by 1)
					ArgumentSource::Constant,
					{ArgumentType::Immediate, 4, 0}
				},
			}
		}
	},
	{	//	ROL Rd	(Rd = Rd rotated left by 1)
		"ROL",
		{TokenType::GREGISTER},
		{&isa[ALU_IMM_OPCODE]},
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
				{	//	This immediate is ignored by ROL
					ArgumentSource::Constant,
					{ArgumentType::Immediate, 4, 0}
				},
			}
		}
	},
	{	//	LSR Rd	(Rd = Rd >> 1 (logical shift right of Rd) )
		"LSR",
		{TokenType::GREGISTER},
		{&isa[ALU_IMM_OPCODE]},
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
				{	//	This immediate is ignored by LSR
					ArgumentSource::Constant,
					{ArgumentType::Immediate, 4, 0}
				},
			}
		}
	},
	{	//	ASR Rd	(Rd = Rd >> 1 (arithmetic shift right of Rd) )
		"ASR",
		{TokenType::GREGISTER},
		{&isa[ALU_IMM_OPCODE]},
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
				{	//	This immediate is ignored by ASR
					ArgumentSource::Constant,
					{ArgumentType::Immediate, 4, 0}
				},
			}
		}
	},
	{	//	ROR Rd	(Rd = Rd rotated right by 1 bit)
		"ROR",
		{TokenType::GREGISTER},
		{&isa[ALU_IMM_OPCODE]},
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
				{	//	This immediate is ignored by ROR
					ArgumentSource::Constant,
					{ArgumentType::Immediate, 4, 0}
				},
			}
		}
	},
	{	//	ADC Rd, Immediate
		"ADC",
		{TokenType::GREGISTER, TokenType::IMMEDIATE},
		{&isa[ALU_IMM_OPCODE]},
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
					{ArgumentType::Immediate, 4, SECOND_TOKEN}
				},
			}
		}
	},
	{	//	SBC Rd, Immediate
		"SBC",
		{TokenType::GREGISTER, TokenType::IMMEDIATE},
		{&isa[ALU_IMM_OPCODE]},
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
					{ArgumentType::Immediate, 4, SECOND_TOKEN}
				},
			}
		}
	},
	{	//	DEC Rd
		"DEC",
		{TokenType::GREGISTER},
		{&isa[ALU_IMM_OPCODE]},
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
					ArgumentSource::Constant,
					{ArgumentType::Immediate, 4, 1}
				},
			}
		}
	},
	{	//	INC Rd
		"INC",
		{TokenType::GREGISTER},
		{&isa[ALU_IMM_OPCODE]},
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
					ArgumentSource::Constant,
					{ArgumentType::Immediate, 4, 1}
				},
			}
		}
	},
	{	//	OUT Rs1, OUT_PRINT_TYPE
		"OUT",
		{TokenType::GREGISTER, TokenType::OUT_PRINT_TYPE},
		{&isa[OUT_REG_OPCODE]},
		{
			{
				{
					ArgumentSource::Token,
					{ArgumentType::Rs1, GREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::OUTDisplayType, 1, SECOND_TOKEN}
				}
			}
		}
	},
	{	//	LMA Rd, #Immediate
		"LMA",
		{TokenType::GREGISTER, TokenType::IMMEDIATE},
		{&isa[LMA_OPCODE]},
		{
			{
				{
					ArgumentSource::Token,
					{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Immediate, 8, SECOND_TOKEN}
				}
			}
		}
	},
	{	//	SMA Rd, #Immediate
		"SMA",
		{TokenType::GREGISTER, TokenType::IMMEDIATE},
		{&isa[SMA_OPCODE]},
		{
			{
				{
					ArgumentSource::Token,
					{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Immediate, 8, SECOND_TOKEN}
				}
			}
		}
	},
	{	//	LMR Rd, Rs1
		"LMR",
		{TokenType::GREGISTER, TokenType::GREGISTER},
		{&isa[LMR_OPCODE]},
		{
			{
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
	{	//	SMR Rd, Rs1
		"SMR",
		{TokenType::GREGISTER, TokenType::GREGISTER},
		{&isa[SMR_OPCODE]},
		{
			{
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
	{	//	OUT #Immediate, OUT_DISPLAY_TYPE
		"OUT",
		{TokenType::IMMEDIATE, TokenType::OUT_PRINT_TYPE},
		{&isa[OUT_IMM_OPCODE]},
		{
			{
				{
					ArgumentSource::Token,
					{ArgumentType::Immediate, 8, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::OUTDisplayType, 1, SECOND_TOKEN}
				}
			}
		}
	},
	{	//	PUSH Rs1
		"PUSH",
		{TokenType::GREGISTER},
		{&isa[PUSH_OPCODE]},
		{
			{
				{
					ArgumentSource::Token,
					{ArgumentType::Rs1, GREGISTER_SIZE, FIRST_TOKEN}
				},
			}
		}
	},
	{	//	POP Rd
		"POP",
		{TokenType::GREGISTER},
		{&isa[POP_OPCODE]},
		{
			{
				{
					ArgumentSource::Token,
					{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN}
				},
			}
		}
	},
	{	//	JSR (for function pointer semantics - i.e., jump to DP)
		//	JSR takes no arguments.
		"JSR",
		{},
		{&isa[JSR_OPCODE]},
		{
			{}
		}
	},
	{	//	A: CALL :label
		//	Implemented as:
		//	1.	Set DP = target address.
		//	2.	JSR.
		//	A:		LDA %DP, L, :label[0]
		//	A + 2:	LDA %DP, H, :label[1]
		//	A + 4:	JSR
		"CALL",
		{TokenType::LABEL},
		{&isa[LDA_OPCODE], &isa[LDA_OPCODE], &isa[JSR_OPCODE]},
		{
			{	//	A:	LDA %DP, L, :label[0]
				{
					ArgumentSource::Constant,
					{ArgumentType::ARDest, AREGISTER_SIZE, DP_BITS}
				},
				{
					ArgumentSource::Constant,
					{ArgumentType::ARHorL, 1, ARHorL_LOW_BITS}
				},
				{
					ArgumentSource::TokenLabelLow,
					{ArgumentType::Immediate, 8, FIRST_TOKEN}
				}
			},
			{	//	A + 2:	LDA %DP, H, :label[1]
				{
					ArgumentSource::Constant,
					{ArgumentType::ARDest, AREGISTER_SIZE, DP_BITS}
				},
				{
					ArgumentSource::Constant,
					{ArgumentType::ARHorL, 1, ARHorL_HIGH_BITS}
				},
				{
					ArgumentSource::TokenLabelHigh,
					{ArgumentType::Immediate, 8, FIRST_TOKEN}
				}
			},
			{	//	A + 4: JSR (no arguments

			}
		}


		//	Implemented as:
		//	A: LDA %DP, H, :label[1]
		//	A + 2: LDR R0, (A + 8)[1]
		//	A + 4: PUSH R0
		//	A + 6: JSR :label[0]
		//	A + 8: ...
		//	NOTE: CALL overrides the value in R0!
		//	TODO: REMOVE THIS IMPLEMENTATION OF CALL WHEN LSP IS REMOVED. CALL
		//	WILL BECOME A WRAPPER FOR JSR WHICH IS USED TO JUMP TO THE DP AR
		//	ADDRESS.
		//	TODO: REMOVE THIS IMPLEMENTATION OF CALL WHEN THE JSR ISA
		//	INSTRUCTION BREAKS DUE TO THE PC[L] -> DB DIRECT CONNECTION BEING
		//	REMOVED (JSR WILL BE UPDATED EVENTUALLY BUT REQUIRES NEW HARDWARE
		//	SUPPORT, E.G. THE AR DATA BUS LOAD ENABLE LUT).	
		/*"CALL",
		{TokenType::LABEL},
		{&isa[LDA_OPCODE], &isa[LDR_OPCODE], &isa[PUSH_OPCODE], 
			&isa[JSR_OPCODE]},
		{
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::ARDest, AREGISTER_SIZE, PC_BITS}
				},
				{
					ArgumentSource::Constant,
					{ArgumentType::ARHorL, 1, ARHorL_HIGH_BITS}
				},
				{
					ArgumentSource::TokenLabelHigh,
					{ArgumentType::Immediate, 8, FIRST_TOKEN}
				},
			},
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::Rd, GREGISTER_SIZE, R0_BITS}
				},
				{
					ArgumentSource::CurrentAddressHighOffset,
					{ArgumentType::Immediate, 8, 4 * ISA_INSTRUCTION_SIZE_BYTES}
				},
			},
			{
				{
					ArgumentSource::Constant,
					{ArgumentType::Rs1, GREGISTER_SIZE, R0_BITS}
				},
			},
			{
				{
					ArgumentSource::TokenLabelLow,
					{ArgumentType::Immediate, 8, FIRST_TOKEN}
				},
			},
		}*/
	},
	{	//	RTS
		"RTS",
		{},
		{&isa[RTS_OPCODE]},
		{{}}
	},
	//	TODO: Add Test Reg and Test Immediate instructions
	//{	//	LSP SREGISTER, #Immediate
	//	//	TODO: Remove this when LSP is removed.
	//	"LSP",
	//	{TokenType::SREGISTER, TokenType::IMMEDIATE},
	//	{&isa[LSP_IMM_OPCODE]},
	//	{
	//		{
	//			{
	//				ArgumentSource::Token,
	//				{ArgumentType::SpecialRegister, SREGISTER_SIZE, FIRST_TOKEN}
	//			},
	//			{
	//				ArgumentSource::Token,
	//				{ArgumentType::Immediate, 8, SECOND_TOKEN}
	//			},
	//		}
	//	}
	//},
	{	//	LDW Rd, %AR, #Imm
		//	Rd = *(ARSrc + Imm)
		"LDW",
		{TokenType::GREGISTER, TokenType::AREGISTER, TokenType::IMMEDIATE},
		{&isa[LDW_OPCODE]},
		{
			{
				{
					ArgumentSource::Token,
					{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN},
				},
				{
					ArgumentSource::Token,
					{ArgumentType::ARSrc, AREGISTER_SIZE, SECOND_TOKEN},		
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Immediate, 8, THIRD_TOKEN}
				}
			}
		}
	},
	{	//	SDW Rs, %AR, #Imm
		//	*(ARSrc + Imm) = Rs (Rd as source)
		"SDW",
		{TokenType::GREGISTER, TokenType::AREGISTER, TokenType::IMMEDIATE},
		{&isa[SDW_OPCODE]},
		{
			{
				{
					ArgumentSource::Token,
					{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN},
				},
				{
					ArgumentSource::Token,
					{ArgumentType::ARSrc, AREGISTER_SIZE, SECOND_TOKEN},
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Immediate, 8, THIRD_TOKEN}
				}
			}
		}
	},
	{	//	LDA ARDest, H/L, #Imm
		//	ARDest[H/L] = Imm
		"LDA",
		{TokenType::AREGISTER, TokenType::ARHorL, TokenType::IMMEDIATE},
		{&isa[LDA_OPCODE]},
		{
			{
				{
					ArgumentSource::Token,
					{ArgumentType::ARDest, AREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::ARHorL, 1, SECOND_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Immediate, 8, THIRD_TOKEN}
				}
			}
		}
	},
	{	//	MGA ARDest, H/L, Rs1
		//	ARDest[H/L] = Rs1
		"MGA",
		{TokenType::AREGISTER, TokenType::ARHorL, TokenType::GREGISTER},
		{&isa[MGA_OPCODE]},
		{
			{
				{
					ArgumentSource::Token,
					{ArgumentType::ARDest, AREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::ARHorL, 1, SECOND_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Rs1, GREGISTER_SIZE, THIRD_TOKEN}
				}
			}
		}
	},
	{	//	MAG Rd, ARSrc, L/H
		//	Rd = ARSrc[L/H]
		"MAG",
		{TokenType::GREGISTER, TokenType::AREGISTER, TokenType::ARHorL},
		{&isa[MAG_OPCODE]},
		{
			{
				{
					ArgumentSource::Token,
					{ArgumentType::Rd, GREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::ARSrc, AREGISTER_SIZE, SECOND_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::ARHorL, 1, THIRD_TOKEN}
				}
			}
		}
	},
	{	//	MAA ARDest, ARSrc, #Imm
		//	ARDest = ARSrc + Imm
		"MAA",
		{TokenType::AREGISTER, TokenType::AREGISTER, TokenType::IMMEDIATE},
		{&isa[MAA_OPCODE]},
		{
			{
				{
					ArgumentSource::Token,
					{ArgumentType::ARDest, AREGISTER_SIZE, FIRST_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::ARSrc, AREGISTER_SIZE, SECOND_TOKEN}
				},
				{
					ArgumentSource::Token,
					{ArgumentType::Immediate, 8, THIRD_TOKEN}
				}
			}
		}
	}
};