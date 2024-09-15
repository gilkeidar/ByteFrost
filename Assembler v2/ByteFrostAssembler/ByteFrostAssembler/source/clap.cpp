#include "clap.hpp"
#include "utility.hpp"
#include <iostream>

CLAP::CLAP(Assembler * assembler, int argc, char** argv) {
	this->assembler = assembler;
	this->argc = argc;
	this->argv = argv;
}

void CLAP::run() {
	std::cout << "Stage 0: CLAP.run()" << std::endl;
	
	//	Convert each command-line argument from a char * (string) w to a
	//	CLToken and add it to a vector<CLToken> tokens
	std::vector<CLToken> tokens;

	for (int i = 1; i < argc; i++) {
		std::string tokenString = argv[i];
		std::cout << "argv[" << std::to_string(i) << "] = " << tokenString 
			<< std::endl;
		CLToken token = stringToCLToken(tokenString);

		std::cout << "TokenType: " << CLTokenTypeToString(token.type) << std::endl;

		//	Check for invalid token type
		if (token.type == CLTokenType::INVALID) {
			std::cerr << "Error: Invalid token " << token.token_string << " detected." << std::endl;
			exit(1);
		}

		tokens.push_back(token);
	}

	//	Iterate over list of tokens. Each token is treated as a flag, a flag
	//	argument, or as the required input argument which is the input file
	//	name.
	//	The first argument that is of type CLTokenType.FILE, ends with ".asm",
	//	and is not an argument of a previous flag token will be undestood to be
	//	the required input file name argument.
	//	When a flag token is encountered, the CLAP will check whether the flag
	//	is a CLAP-recognized flag by checking whether the token_string exists
	//	as a key of the flag name to CFLAG hashmap in the CommandLineArguments
	//	bject.
}

CLToken stringToCLToken(std::string w) {
	//	Given a string w, match it with a CLTokenType, and return a CLToken
	//	containing w and the matched CLTokenType.

	CLTokenType type = CLTokenType::INVALID;
	
	//	CLTokenTypes to match with w:
	//	1.	FLAG
	//		w is mapped to CLTokenType.FLAG iff:
	//			1.	w.length() >= 2
	//			2.	w[0] = '-'
	//			3.	w[1:end] is in TEXT
	//	2.	NUMBER
	//		w is mapped to CLTokenType.NUMBER iff w is in NUMBER.
	//	3.	FILE_NAME
	//		w is mapped to CLTokenType.FILE_NAME iff w is in FILE
	//	4.	TEXT
	//		w is mapepd to CLTokenType.TEXT iff w is in TEXT
	//	5.	INVALID (matched with s if s doesn't match any from 1 - 4)

	//	Map w to a CLTokenType
	if (w.length() >= 2 && w[0] == '-' && isTEXTString(w.substr(1))) {
		//std::cout << "FLAG" << std::endl;
		type = CLTokenType::FLAG;
	}
	else if (isNUMBERString(w)) {
		//std::cout << "NUMBER" << std::endl;
		type = CLTokenType::NUMBER;
	}
	else if (isFILEString(w)) {
		//std::cout << "FILE_NAME" << std::endl;
		type = CLTokenType::FILE_NAME;
	}
	else if (isTEXTString(w)) {
		//std::cout << "TEXT" << std::endl;
		type = CLTokenType::TEXT;
	}
	else if (type == CLTokenType::INVALID) {
		//std::cout << "INVALID" << std::endl;
		type = CLTokenType::INVALID;
	}

	return {type, w};
}

std::string CLTokenTypeToString(CLTokenType type) {
	switch (type) {
		case CLTokenType::FLAG:
			return "FLAG";
		case CLTokenType::NUMBER:
			return "NUMBER";
		case CLTokenType::FILE_NAME:
			return "FILE_NAME";
		case CLTokenType::TEXT:
			return "TEXT";
		case CLTokenType::INVALID:
			return "INVALID";
		default:
			return "UNKNOWN TYPE";
	}
}