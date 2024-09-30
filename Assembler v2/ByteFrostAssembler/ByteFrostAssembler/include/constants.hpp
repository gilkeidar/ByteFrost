#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>

/*	Debug Mode	*/
//	Note to the Programmer: Use this constant as a toggle for debug print outs.
//	Debug printouts must use the debug() function defined in utility.hpp to be
//	affected by this debug toggle guard.
#define	DEBUG_PRINTOUTS	false

//	TODO: Maybe add a void verbose(std::string) function in utility.hpp for
//	printouts that are only printed when a -verbose flag is set.

//	TODO: Maybe add a RUN_TEST_SUITE #define here and add a test suite that
//	would be run at the start of main() if it is set to true.

/*	Stage 0: CLAP Constants	*/

//	Command-Line Arguments
const std::string UNSET_FILE_NAME = "UNSET";
const std::string ASSEMBLY_FILE_ENDING = "asm";

#define	BINARY_FLAG_NAME		"b"
#define OUTPUT_FILE_FLAG_NAME	"o"

/*	Stage 1: Parser Constants	*/

//	Parser (Stage 1) Delimiters (.asm line delimiters)
//	Note: These are added by the Parser to its delimiters set in its 
//	constructor
#define	TOKEN_DELIMITER_SPACE	' '
#define	TOKEN_DELIMITER_TAB		'\t'
#define	TOKEN_DELIMITER_COMMA	','

//	Parsing
#define	UNDERSCORE				'_'
#define	SPECIAL_REGISTER_PREFIX	'%'
#define	IMMEDIATE_PREFIX		'#'
#define	DIRECTIVE_PREFIX		'.'
#define	LABEL_PREFIX			':'
#define	BYTE_SELECT_START		'['
#define	BYTE_SELECT_END			']'

const std::string COMMENT_START = "//";

//	Print types (used by OUT instruction)
const std::string ASCII_PRINT_TYPE = "A";
const std::string INTEGER_PRINT_TYPE = "I";

//	General Register Strings
const std::string GREGISTER_R0_NAME = "R0";
const std::string GREGISTER_R1_NAME = "R1";
const std::string GREGISTER_R2_NAME = "R2";
const std::string GREGISTER_R3_NAME = "R3";

//	Special Register Strings
const std::string SREGISTER_DHPC_NAME = "DHPC";
const std::string SREGISTER_HDP_NAME = "HDP";
const std::string SREGISTER_HSP_NAME = "HSP";

//	Instruction Arguments

//	Argument Source

//	Token Source (1-indexed as tokens[0] is the instruction name)
#define	FIRST_TOKEN		1
#define	SECOND_TOKEN	2
#define	THIRD_TOKEN		3
#define	FOURTH_TOKEN	4

//	Instruction Argument Sizes
#define	GREGISTER_SIZE	2
#define	SREGISTER_SIZE	2
#define	ALU_FUNC_SIZE	4
#define	BRANCH_COND_SIZE	3

//	Instruction Argument Default Position
#define	RD_DEF_POSITION		6
#define	RS1_DEF_POSITION	12
#define	RS2_DEF_POSITION	14

//	Function Bit Values
#define	OR_FUNC_BITS	0
#define	AND_FUNC_BITS	2
#define	XOR_FUNC_BITS	4
#define	NOT_FUNC_BITS	5
#define	ADD_FUNC_BITS	6
#define	SUB_FUNC_BITS	7
#define	ASL_FUNC_BITS	8
#define ROL_FUNC_BITS	9
#define	LSR_FUNC_BITS	0xa
#define	ASR_FUNC_BITS	0xb
#define	ROR_FUNC_BITS	0xc
#define	ADC_FUNC_BITS	0xe
#define	SBC_FUNC_BITS	0xf

//	Branch Condition Bit Values
#define JMP_COND_BITS	0
#define	BMI_COND_BITS	1	//	Branch on Minus (negative flag)
#define	BCS_COND_BITS	2	//	Branch on Carry Set (carry flag)
#define	BEQ_COND_BITS	3	//	Branch Equal (zero flag)
#define	BPL_COND_BITS	5	//	Branch on Plus (nonnegative) (negative flag not set)
#define	BCC_COND_BITS	6	//	Branch on Carry Clear (carry flag not set)
#define	BNE_COND_BITS	7	//	Branch Not Equal (zero flag not set)

//	General Register Values
#define	R0_BITS			0
#define	R1_BITS			1
#define	R2_BITS			2
#define	R3_BITS			3

//	Special Register Values
#define	DHPC_BITS		0
#define	HDP_BITS		1
#define	HSP_BITS		2


const std::unordered_map<std::string, uint8_t> general_register_bits({
	{GREGISTER_R0_NAME, R0_BITS},
	{GREGISTER_R1_NAME, R1_BITS},
	{GREGISTER_R2_NAME, R2_BITS},
	{GREGISTER_R3_NAME, R3_BITS}
	});

/**
 * @brief Special register name (string) -> bit values (uint8_t)
 */
const std::unordered_map<std::string, uint8_t> special_register_bits({
	{SREGISTER_DHPC_NAME, DHPC_BITS},
	{SREGISTER_HDP_NAME, HDP_BITS},
	{SREGISTER_HSP_NAME, HSP_BITS}
	});

//	OUT Print Type Values
#define	ASCII_PRINT_TYPE_BIT	0
#define	INTEGER_PRINT_TYPE_BIT	1

//	Assembly Instructions

//	ISA Instructions

//	Size of ISA Instructions in bytes
#define	ISA_INSTRUCTION_SIZE_BYTES	2

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

//	Address confinements
//	Address space is 16-bit (64KB), and is divided as follows:
//	FFFF	MMIO
//	|		MMIO
//	E000	MMIO
//	DFFF	RAM
//	|		RAM
//	2000	RAM
//	1999	ROM
//	|		ROM
//	0000	ROM
//	Can only store instructions in ROM and RAM regions, but each program can
//	only be written for one region (ROM or RAM).
#define	MIN_ADDRESS		0x0000
#define	MAX_ADDRESS		0xDFFF
#define	ROM_START_ADDRESS	0x0000
#define	ROM_END_ADDRESS		0x1999
#define	RAM_START_ADDRESS	0x2000
#define	RAM_END_ADDRESS		0xDFFF
#define	ROM_SIZE_BYTES	(8 * 1024)
#define	RAM_SIZE_BYTES	(48 * 1024)

//	Preprocessor

//	A PreprocessorConstant may at most be 8 bytes in size, as the value is
//	stored in a long.
//	e.g., .define 16 VERY_LARGE_CONST	1701
//	should result in an error in Preprocessor.handleDefineDirective() as the
//	constant size is specified as 16 bytes.
#define	PREPROCESSOR_CONSTANT_MAX_SIZE	8

//	Label Handler

//	Each label stores a 16-bit (2-byte) address. Hence, its size is 2 bytes.
#define	LABEL_SIZE_BYTES	2