#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include "clap.hpp"

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
};

class Parser {
public:
	Parser();

	std::vector<Line *> run(CommandLineArguments* arguments);
private:
	std::unordered_set<char> delimiters;
	std::unordered_set<std::string> general_registers;
	std::unordered_set<std::string> special_registers;

	Token stringToToken(std::string w);
};