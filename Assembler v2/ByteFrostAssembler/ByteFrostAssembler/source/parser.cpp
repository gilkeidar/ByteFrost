#include "parser.hpp"
#include "clap.hpp"
#include "utility.hpp"
#include "constants.hpp"
#include "assembly_instructions.hpp"
#include <iostream>
#include <fstream>

Parser::Parser() {
	//	Initialize delimiter set to {' ', '\t', ','}
	this->delimiters.insert(' ');
	this->delimiters.insert('\t');
	this->delimiters.insert(',');

	//	Initialize general purpose registers set
	this->general_registers.insert("R0");
	this->general_registers.insert("R1");
	this->general_registers.insert("R2");
	this->general_registers.insert("R3");

	//	Initialize special purpose registers set
	this->special_registers.insert("DPCH");	//	Dummy PC High Byte
	this->special_registers.insert("SPH");	//	SP High Byte
	this->special_registers.insert("DPH");	//	DP High Byte
}

std::vector<Line*> Parser::run(CommandLineArguments* arguments,
	std::unordered_map<std::string, std::vector<AssemblyInstruction>> & instructions) {
	std::cout << "=== Stage 1: Parser.run() ===" << std::endl;

	//	1.	Open the input file
	std::ifstream input_file;

	input_file.open(arguments->input_file_name, std::ifstream::in);

	if (!input_file.is_open()) {
		throwError("Could not open file '" + arguments->input_file_name
			+ "' for reading.");
	}

	std::cout << "file is opened for reading\n";

	//	2.	Generate a std::vector<std::string> line_strings vector from the
	//		input file.
	std::vector<std::string> line_strings;

	std::string currentString;
	//int i = 1;
	while (input_file.good()) {
		//std::cout << "line number: " << std::to_string(i++) << std::endl;
		//currentString = "";
		std::getline(input_file, currentString);

		/*for (int j = 0; j < currentString.length(); j++) {
			int asciiValue = currentString[j];
			std::cout << "character: " << currentString[j] << "\t|\tvalue: " << std::to_string(asciiValue) << std::endl;
		}*/

		//std::cout << "Read: " << currentString << std::endl;
		if (currentString.length() > 0) {
			line_strings.push_back(currentString);
		}
	}

	input_file.close();

	std::cout << "file is closed\n";

	for (int i = 0; i < line_strings.size(); i++) {
		std::cout << "Line " << std::to_string(i + 1) << ": " << line_strings[i] << std::endl;
	}

	//	3.	Generate a std::vector<Line *> lines vector
	
	std::vector<Line*> lines;

	//for (std::string s : line_strings) {
	for (int line_count = 0; line_count < line_strings.size(); line_count++) {
		std::string s = line_strings[line_count];
		std::cout << "== CONVERTING s to Line * ==" << std::endl;
		std::cout << "s = " << s << std::endl;

		//	3.1	Generate a string line_string that contains no comments.
		size_t indexComments = s.find(COMMENT_START);

		std::string line_string;

		if (indexComments != std::string::npos) {
			//	Comment found, remove it
			line_string = s.substr(0, indexComments);
		}
		else {
			//	No comment found; maintain s as is
			line_string = s;
		}

		std::cout << "s' = " << line_string << std::endl;

		//	3.2	Generate a std::vector<string> token_strings vector.
		std::vector<std::string> token_strings;
		bool isTokenString = false;
		std::string currTokenString = "";
		for (int i = 0; i < line_string.length(); i++) {
			char c = line_string[i];
			if (this->delimiters.find(c) != this->delimiters.end()) {
				//	c is a delimiter
				isTokenString = false;
			}
			else {
				isTokenString = true;
			}

			if (isTokenString) {
				currTokenString.push_back(c);
			}
			else if (currTokenString.length() > 0) {
				token_strings.push_back(currTokenString);
				currTokenString = "";
			}
		}
		if (isTokenString) {
			token_strings.push_back(currTokenString);
		}

		for (std::string s : token_strings) {
			std::cout << "Token string: '" << s << "'" << std::endl;
		}

		//	3.3	Generate a std::vector<Token> tokens vector.
		std::vector<Token> tokens;

		for (std::string w : token_strings) {
			Token t = stringToToken(w, instructions);
			//	Throw error if token is invalid
			if (t.type == TokenType::INVALID) {
				throwError("Invalid token '" + t.token_string + "' in line "
					+ std::to_string(line_count + 1) + ": " + s);
			}
			tokens.push_back(t);
		}

		for (Token t : tokens) {
			std::cout << "Token: " << TokenToString(t) << std::endl;
		}

		//	3.4	Generate a Line object and add it to the vector<Line *> lines.
		Line* line = generateLine(s, tokens, instructions);

		//	Print line found

		std::cout << "Line: " << LineToString(line) << std::endl;

		//	Check that line isn't invalid
		if (line->type == LineType::INVALID) {
			throwError("Line " + std::to_string(line_count + 1) 
				+ " is invalid.");
		}


		lines.push_back(line);
	}

	return lines;
}

Token Parser::stringToToken(std::string w, 
	std::unordered_map<std::string, std::vector<AssemblyInstruction>> & instructions) {
	//	Given a string w, match it with a TokenType and return a Token 
	//	containing w and the matched TokenType.

	TokenType type;

	if (isTEXTString(w)) {
		if (instructions.find(w) != instructions.end()) {
			//	TODO: Check whether w is an instruction (need Assembler's
			//	std::string -> std::vector<Instruction> hashmap to exist)
			type = TokenType::INSTRUCTION;
		}
		else if (this->general_registers.find(w) != this->general_registers.end()) {
			//	Check whether w is a general purpose register
			type = TokenType::GREGISTER;
		}
		else {
			//	Otherwise, w is TEXT
			type = TokenType::TEXT;
		}
	}
	else if (w.length() >= 2 && w[0] == SPECIAL_REGISTER_PREFIX &&
			this->special_registers.find(w.substr(1)) != this->special_registers.end()) {
		//	w is a special register
		type = TokenType::SREGISTER;
	}
	else if (isNUMBERString(w)) {
		//	w is a number
		type = TokenType::NUMBER;
	}
	else if (w.length() >= 2 && w[0] == IMMEDIATE_PREFIX &&
		isNUMBERString(w.substr(1))) {
		//	w is an immediate
		type = TokenType::IMMEDIATE;
	}
	else if (w.length() >= 2 && w[0] == DIRECTIVE_PREFIX 
		&& isTEXTString(w.substr(1))) {
		//	w is a preprocessor directive
		type = TokenType::DIRECTIVE;
	}
	else if (w.length() >= 2 && w[0] == LABEL_PREFIX 
		&& isTEXTString(w.substr(1))) {
		//	w is a label
		type = TokenType::LABEL;
	}
	else {
		//	w is invalid
		type = TokenType::INVALID;
	}

	return { type, w };
}

std::string TokenTypeToString(TokenType t) {
	switch (t) {
	case TokenType::INSTRUCTION:
		return "INSTRUCTION";
	case TokenType::GREGISTER:
		return "GREGISTER";
	case TokenType::TEXT:
		return "TEXT";
	case TokenType::SREGISTER:
		return "SREGISTER";
	case TokenType::NUMBER:
		return "NUMBER";
	case TokenType::IMMEDIATE:
		return "IMMEDIATE";
	case TokenType::DIRECTIVE:
		return "DIRECTIVE";
	case TokenType::LABEL:
		return "LABEL";
	case TokenType::INVALID:
		return "INVALID";
	default:
		return "UNKNOWN_TYPE";
	}
}

std::string TokenToString(Token t) {
	return "{type: "
		+ TokenTypeToString(t.type)
		+ ", token_string: "
		+ t.token_string + "}";
}

Line* Parser::generateLine(std::string s, std::vector<Token> tokens,
	std::unordered_map<std::string, std::vector<AssemblyInstruction>> & instructions) {
	//	Given vector<Token> tokens, allocate and return a Line object.
	Line* line = nullptr;

	//	Identify line type
	if (tokens.size() == 0) {
		//	Line has no tokens - type is Empty
		return new Line(LineType::EMPTY, s, tokens);
	}

	//	Line has at least one token
	if (tokens[0].type == TokenType::INSTRUCTION) {
		//	Line may be an instruction - verify that the rest of the tokens
		//	match the expected argument sequence of an instruction with the
		//	name tokens[0].token_string
		std::vector<AssemblyInstruction> & possibleInstrs =
			instructions[tokens[0].token_string];

		for (AssemblyInstruction& instruction : possibleInstrs) {
			if (matchesInstructionArgs(tokens, instruction)) {
				return new InstructionLine(s, tokens, &instruction);
			}
		}

		//	No valid instruction found
		throwError("Incorrect arguments for instruction '"
			+ tokens[0].token_string + "'.");
	}
	//	TODO: Add checks for Preprocessor directives and label definitions here
	else {
		//	Invalid line
		return new Line(LineType::INVALID, s, tokens);
	}
}

bool Parser::matchesInstructionArgs(std::vector<Token> tokens, AssemblyInstruction& instruction) {
	//	Check whether token types match the instruction's expected arguments
	//	For this to be true:
	//	1.	tokens.size() - 1 == instruction->expected_param_types.size().
	//	2.	tokens[0].type == TokenType::INSTRUCTION.
	//	3.	for i = 0 to instruction->expected_param_types.size():
	//			tokens[i + 1].type matches expected_param_type[i]

	if (tokens.size() - 1 != instruction.expected_param_types.size())
		return false;

	if (tokens[0].type != TokenType::INSTRUCTION)
		return false;

	//	"Quantity" type arguments may be immediates, Labels, or constants
	//	(which are TEXT tokens that the parser will assume are the name of 
	//	preprocessor constants - if it isn't, then the preprocessor will throw
	//	an error).
	std::unordered_set<TokenType> quantity_types({ TokenType::IMMEDIATE,
		TokenType::LABEL, TokenType::TEXT });

	for (int i = 0; i < instruction.expected_param_types.size(); i++) {
		if ((quantity_types.find(tokens[i + 1].type) != quantity_types.end())
			&&
			(quantity_types.find(instruction.expected_param_types[i]) != quantity_types.end()))
		{
			//	Both argument types are quantity types
			continue;
		}
		if (tokens[i + 1].type != instruction.expected_param_types[i]) {
			//	found arguments that don't match in type
			return false;
		}
	}

	return true;
}

std::string LineTypeToString(LineType t) {
	switch (t) {
	case LineType::INSTRUCTION:
		return "INSTRUCTION";
	case LineType::DIRECTIVE:
		return "DIRECTIVE";
	case LineType::LABEL_DEFINITION:
		return "LABEL_DEFINITION";
	case LineType::EMPTY:
		return "EMPTY";
	case LineType::INVALID:
		return "INVALID";
	default:
		return "UNKNOWN_TYPE";
	}
}

std::string LineToString(Line* line) {
	return "{ type: " + LineTypeToString(line->type) + "}";
}