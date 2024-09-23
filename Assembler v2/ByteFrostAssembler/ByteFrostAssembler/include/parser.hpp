#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "clap.hpp"

//	Forward declaration of AssemblyInstruction struct
struct AssemblyInstruction;

enum class TokenType {INSTRUCTION, GREGISTER, TEXT, SREGISTER, NUMBER, IMMEDIATE, 
	DIRECTIVE, LABEL, BYTE_CONSTANT, BYTE_LABEL, INVALID};

struct Token {
	TokenType type;
	std::string token_string;
};

std::string TokenTypeToString(TokenType t);

std::string TokenToString(Token t);

enum class LineType {INSTRUCTION, DIRECTIVE, LABEL_DEFINITION, EMPTY, INVALID};

struct Line {
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

	Line(LineType type, std::string original_string, std::vector<Token> tokens,
		uint16_t line_address)
		: type(type), original_string(original_string), tokens(tokens),
			line_address(line_address) {}
};

std::string LineTypeToString(LineType t);

std::string LineToString(Line* line);

struct InstructionLine : Line {
	//	Assembly Instruction of given Line
	AssemblyInstruction* instruction;

	InstructionLine(std::string original_string, std::vector<Token> tokens,
		uint16_t line_address, AssemblyInstruction* instruction) 
		: Line(LineType::INSTRUCTION, original_string, tokens, line_address), 
			instruction(instruction) {}
};

class Parser {
public:
	Parser();

	std::vector<Line *> run(CommandLineArguments* arguments, 
		std::unordered_map<std::string, std::vector<AssemblyInstruction>> & instructions);
private:
	std::unordered_set<char> delimiters;
	std::unordered_set<std::string> general_registers;
	std::unordered_set<std::string> special_registers;

	Token stringToToken(std::string w, 
		std::unordered_map<std::string, std::vector<AssemblyInstruction>> & instructions);

	Line* generateLine(std::string s, std::vector<Token> tokens,
		std::unordered_map<std::string, 
		std::vector<AssemblyInstruction>> & instructions, uint16_t & current_address);

	bool matchesInstructionArgs(std::vector<Token> tokens, AssemblyInstruction & instruction);

	uint16_t getNextLineAddress(uint16_t current_address, AssemblyInstruction & instruction);
};