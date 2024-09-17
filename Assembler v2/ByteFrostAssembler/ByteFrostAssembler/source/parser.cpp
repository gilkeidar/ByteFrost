#include "parser.hpp"
#include "clap.hpp"
#include "utility.hpp"
#include "constants.hpp"
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

std::vector<Line*> Parser::run(CommandLineArguments* arguments) {
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

	for (std::string s : line_strings) {
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
			tokens.push_back(stringToToken(w));
		}

		for (Token t : tokens) {
			std::cout << "Token: " << TokenToString(t) << std::endl;
		}
	}

	return {};
}

Token Parser::stringToToken(std::string w) {
	//	Given a string w, match it with a TokenType and return a Token 
	//	containing w and the matched TokenType.

	TokenType type;

	if (isTEXTString(w)) {
		if (false) {
			//	TODO: Check whether w is an instruction (need Assembler's
			//	std::string -> std::vector<Instruction> hashmap to exist)
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