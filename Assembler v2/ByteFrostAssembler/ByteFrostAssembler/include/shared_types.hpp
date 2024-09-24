#pragma once
#include <string>
#include <vector>
//	Shared types used accross the assembler code

//	Forward declaration of AssemblyInstruction struct
struct AssemblyInstruction;

//	Forward declaration of Preprocessor Directive struct
struct Directive;

enum class TokenType {
	INSTRUCTION, GREGISTER, TEXT, SREGISTER, NUMBER, IMMEDIATE,
	DIRECTIVE, LABEL, BYTE_CONSTANT, BYTE_LABEL, INVALID
};

struct Token {
	TokenType type;
	std::string token_string;
};

std::string TokenTypeToString(TokenType t);

std::string TokenToString(Token t);

enum class LineType { INSTRUCTION, DIRECTIVE, LABEL_DEFINITION, EMPTY, INVALID };

struct Line {
	/**
	 * @brief Line number (1-indexed)
	 * @note This quantity is 1-indexed, so the first Line is line 1, not line
	 * 0.
	 */
	unsigned int number;
	LineType type;
	std::string original_string;
	std::vector<Token> tokens;

	/**
	 * @brief This is the memory address of this line in memory.
	 * @note This address assumes that the address of the first instruction is
	 * 0, and so may not represent the true address until the preprocessor
	 * alters it if needed (if a preprocessor directive was specified to shift
	 * the starting address forward, e.g. for RAM programs).
	 * @note This is an 16-bit ADDRESS, NOT a PC value! The PC value of this
	 * address is the address logically shifted right by 1.
	 */
	uint16_t line_address;

	Line(unsigned int number, LineType type, std::string original_string, std::vector<Token> tokens,
		uint16_t line_address)
		: number(number), type(type), original_string(original_string), tokens(tokens),
		line_address(line_address) {}
};

std::string LineTypeToString(LineType t);

std::string LineToString(Line* line);

struct InstructionLine : Line {
	//	Assembly Instruction of given Line
	AssemblyInstruction* instruction;

	InstructionLine(unsigned int number, std::string original_string, std::vector<Token> tokens,
		uint16_t line_address, AssemblyInstruction* instruction)
		: Line(number, LineType::INSTRUCTION, original_string, tokens, line_address),
		instruction(instruction) {}
};

struct DirectiveLine : Line {
	//	Directive of given line
	Directive* directive;

	DirectiveLine(unsigned int number, std::string original_string, std::vector<Token> tokens,
		uint16_t line_address, Directive* directive)
		: Line(number, LineType::DIRECTIVE, original_string, tokens, line_address),
		directive(directive) {}
};