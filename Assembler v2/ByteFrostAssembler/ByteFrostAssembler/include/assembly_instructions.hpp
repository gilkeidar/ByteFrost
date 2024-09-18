#pragma once
#include <string>
#include <vector>
#include "parser.hpp"

struct AssemblyInstruction {
	std::string name;
	std::vector<TokenType> expected_param_types;
};


const AssemblyInstruction assembly_instructions[] = {
	{"ADD", {TokenType::GREGISTER, TokenType::GREGISTER, TokenType::GREGISTER}},
	{"ADD", {TokenType::GREGISTER, TokenType::IMMEDIATE}},
	{"NOP", {}}
};