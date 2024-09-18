#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "clap.hpp"

//	Forward declaration of AssemblyInstruction struct
struct AssemblyInstruction;

enum class TokenType {INSTRUCTION, GREGISTER, TEXT, SREGISTER, NUMBER, IMMEDIATE, 
	DIRECTIVE, LABEL, INVALID};

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

	Line(LineType type, std::string original_string, std::vector<Token> tokens)
		: type(type), original_string(original_string), tokens(tokens) {}
};

std::string LineTypeToString(LineType t);

std::string LineToString(Line* line);

struct InstructionLine : Line {
	//	Assembly Instruction of given Line
	AssemblyInstruction* instruction;

	InstructionLine(std::string original_string, std::vector<Token> tokens,
		AssemblyInstruction* instruction) : Line(LineType::INSTRUCTION,
			original_string, tokens), instruction(instruction) {}
};

class Parser {
public:
	Parser();

	std::vector<Line *> run(CommandLineArguments* arguments, 
		std::unordered_map<std::string, std::vector<AssemblyInstruction>> instructions);
private:
	std::unordered_set<char> delimiters;
	std::unordered_set<std::string> general_registers;
	std::unordered_set<std::string> special_registers;

	Token stringToToken(std::string w, 
		std::unordered_map<std::string, std::vector<AssemblyInstruction>> instructions);

	Line* generateLine(std::string s, std::vector<Token> tokens,
		std::unordered_map<std::string, std::vector<AssemblyInstruction>> instructions);

	bool matchesInstructionArgs(std::vector<Token> tokens, AssemblyInstruction & instruction);
};