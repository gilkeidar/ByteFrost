#pragma once
#include <string>
//#include "parser.hpp"
#include "clap.hpp"
#include "shared_types.hpp"

enum class DirectiveType {
	define,
	start,
	ram,
	rom
};

struct Directive {
	std::string name;
	DirectiveType type;
	std::vector<TokenType> expected_param_types;
};

struct PreprocessorConstant {
	/**
	 * @brief PreprocessorConstant name
	 */
	std::string name;
	/**
	 * @brief PreprocessorConstant size, in bytes
	 */
	size_t size;

	/**
	 * @brief value of PreprocessorConstant
	 * @note TODO - constant value is capped at long size (8 bytes); need to 
	 * ensure that PreprocessorConstants are not declared with a size larger 
	 * than this.
	 */
	long value;
};

class Preprocessor {
public:
	Preprocessor();
	void run(std::vector<Line*>& lines, CommandLineArguments& args);
private:
	//	Hashmap of string -> PreprocessorConstant
	std::unordered_map<std::string, PreprocessorConstant> constants;

	//void handleDirective(DirectiveLine* line);

	//	Directive handlers
	//void handleDefineDirective(DirectiveLine* line);
};

//	Directives array
const Directive preprocessor_directives[] = {
	{	//	.define CONST_SIZE CONST_NAME CONST_VALUE
		"define",
		DirectiveType::define,
		{TokenType::NUMBER, TokenType::TEXT, TokenType::NUMBER}
	}
};