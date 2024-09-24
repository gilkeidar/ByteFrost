#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "clap.hpp"
#include "shared_types.hpp"
#include "assembly_instructions.hpp"
#include "preprocessor.hpp"

class Parser {
public:
	Parser();

	std::vector<Line *> run(CommandLineArguments* arguments, 
		std::unordered_map<std::string, 
		std::vector<AssemblyInstruction>> & instructions,
		std::unordered_map<std::string, Directive> & directives);
private:
	std::unordered_set<char> delimiters;
	std::unordered_set<std::string> general_registers;
	std::unordered_set<std::string> special_registers;

	Token stringToToken(std::string w, 
		std::unordered_map<std::string, std::vector<AssemblyInstruction>> & instructions);

	Line* generateLine(unsigned int line_number,
		std::string s, 
		std::vector<Token> tokens,
		std::unordered_map<std::string, std::vector<AssemblyInstruction>> & instructions, 
		std::unordered_map<std::string, Directive> & directives,
		uint16_t & current_address);

	bool matchesInstructionArgs(std::vector<Token> tokens, AssemblyInstruction & instruction);

	bool matchesDirectiveArgs(std::vector<Token> tokens, Directive& directive);

	uint16_t getNextLineAddress(uint16_t current_address, AssemblyInstruction & instruction);
};