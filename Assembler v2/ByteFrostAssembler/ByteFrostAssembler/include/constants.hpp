#pragma once
#include <string>

//	Command-Line Arguments
const std::string UNSET_FILE_NAME = "UNSET";
const std::string ASSEMBLY_FILE_ENDING = "asm";

#define	BINARY_FLAG_NAME		"b"
#define OUTPUT_FILE_FLAG_NAME	"o"

//	Parsing
#define	UNDERSCORE				'_'
#define	SPECIAL_REGISTER_PREFIX	'%'
#define	IMMEDIATE_PREFIX		'#'
#define	DIRECTIVE_PREFIX		'.'
#define	LABEL_PREFIX			':'
const std::string COMMENT_START = "//";

//	Instruction Arguments

//	Instruction Argument Sizes
#define	GREGISTER_SIZE	2
#define	SREGISTER_SIZE	2
#define	ALU_FUNC_SIZE	4
#define	BRANCH_COND_SIZE	3

//	Instruction Argument Default Position
#define	RD_DEF_POSITION		6
#define	RS1_DEF_POSITION	12
#define	RS2_DEF_POSITION	14

//	Assembly Instructions

//	ISA Instruction

//	NOP
#define	NOP_OPCODE	0

//	BRK
#define	BRK_OPCODE	1

//	ALU
#define	ALU_OPCODE	2

//	Load Immediate to Reg (LDR)
#define	LDR_OPCODE	3

//	MOV
#define	MOV_OPCODE	4

//	Branch Absolute Immediate
#define	BRANCH_ABS_IMM_OPCODE	5

//	ALU Immediate
#define	ALU_IMM_OPCODE	6

//	Branch Relative Immediate
#define	BRANCH_REL_IMM_OPCODE	7

//	OUT	Register
#define	OUT_REG_OPCODE	8

//	Load Memory Absolute
#define	LMA_OPCODE	9

//	Store Memory Absolute
#define	SMA_OPCODE	0xa

//	Load Memory Register
#define	LMR_OPCODE	0xb

//	Store Memory Register
#define	SMR_OPCODE	0xc

//	OUT Immediate
#define	OUT_IMM_OPCODE	0xd

//	PUSH
#define	PUSH_OPCODE		0xe

//	POP
#define	POP_OPCODE		0xf

//	JSR
#define	JSR_OPCODE		0x10

//	RTS
#define	RTS_OPCODE		0x11

//	Test Reg
#define	TST_REG_OPCODE	0x12

//	Test Immediate
#define	TST_IMM_OPCODE	0x13

//	Load Special Register Immediate
#define	LSP_IMM_OPCODE	0x14