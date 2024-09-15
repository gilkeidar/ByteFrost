#include "clap.hpp"
#include "constants.hpp"
#include "utility.hpp"
#include <iostream>

CLAP::CLAP(int argc, char** argv) {
	this->argc = argc;
	this->argv = argv;
}

CommandLineArguments * CLAP::run() {
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
			throwError("Invalid token '" + token.token_string + "' detected.");
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
	//	object.
	bool flagArguments = false;
	int currentFlagArgument = -1;
	CLFlag * current_flag = nullptr;
	//	Create a CommandLineArguments object
	CommandLineArguments* arguments = new CommandLineArguments();

	for (CLToken token : tokens) {
		//	Check whether token is the required input file name argument
		if (!flagArguments && token.type == CLTokenType::FILE_NAME
			&& stringEndsWith(token.token_string, ASSEMBLY_FILE_ENDING)) {
			arguments->input_file_name = token.token_string;
		}
		//	Check whether token is a flag
		else if (!flagArguments && token.type == CLTokenType::FLAG) {
			//	Check that flag is recognized by the assembler
			std::string flag_name = getFlagName(token.token_string);

			if ((arguments->flags.find(flag_name)) == arguments->flags.end()) {
				//	Flag is not recognized by the assembler
				throwError("'" + flag_name + "' is not a recognized command-line flag.");
			}

			//	Flag is recognized by the assembler; set current_flag field
			current_flag = &(arguments->flags[flag_name]);

			//	If there are any expected arguments, set arguments field and
			//	the current flag argument index
			if (current_flag->expected_pattern.size() > 0) {
				current_flag->is_set = true;
				flagArguments = true;
				currentFlagArgument = 0;
			}
		}
		else if (flagArguments) {
			//	Check that the current token's type matches the expected flag
			//	argument type
			if (token.type != current_flag->expected_pattern[currentFlagArgument]) {
				throwError("Argument " + std::to_string(currentFlagArgument)
					+ " of flag -" + current_flag->flag_name + " must be a "
					+ CLTokenTypeToString(current_flag->expected_pattern[currentFlagArgument]) 
					+ ".");
			}
			else {
				//	Set this flag's value in the CommandLineArguments object.
				current_flag->values.push_back(token);
				currentFlagArgument++;
				if (currentFlagArgument == current_flag->expected_pattern.size()) {
					//	Went through this flag's last argument
					flagArguments = false;
					current_flag = nullptr;
				}
			}
		}
		else {
			//	Invalid argument
			throwError("Invalid argument detected: '" + token.token_string + "'.");
		}
	}

	//	Check that the last flag, if any, received all of its expected
	//	arguments
	if (flagArguments && currentFlagArgument < current_flag->expected_pattern.size()) {
		throwError("Expected "
			+ std::to_string(current_flag->expected_pattern.size())
			+ " arguments for -" + current_flag->flag_name
			+ " flag, but received " + std::to_string(currentFlagArgument) 
			+ " arguments.");
	}

	//	Check that the required input file name was set
	if (arguments->input_file_name == UNSET_FILE_NAME) {
		throwError("Must pass a '.asm' input file.");
	}

	return arguments;
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

CommandLineArguments::CommandLineArguments() {
	//	Initialize input_file_name
	this->input_file_name = UNSET_FILE_NAME;

	//	Initialize flags hashmap

	//	Add Binary Flag "-b"
	CLFlag binary_flag = { BINARY_FLAG_NAME, {}, {}, false };
	this->flags.insert({ BINARY_FLAG_NAME, binary_flag });
	//	Add Output File Name Flag "-o"
	CLFlag output_file_name_flag = { OUTPUT_FILE_FLAG_NAME, {CLTokenType::FILE_NAME}, {}, false };
	this->flags.insert({ OUTPUT_FILE_FLAG_NAME, output_file_name_flag });
}