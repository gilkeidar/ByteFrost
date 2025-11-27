#include "parser.hpp"
#include "clap.hpp"
#include "utility.hpp"
#include "constants.hpp"
#include "assembly_instructions.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

Parser::Parser(Config & config) : config(config) {
	//	Initialize delimiter set to {' ', '\t', ','}
	this->delimiters.insert(TOKEN_DELIMITER_SPACE);
	this->delimiters.insert(TOKEN_DELIMITER_TAB);
	this->delimiters.insert(TOKEN_DELIMITER_COMMA);
}

void Parser::run(std::vector<Line *> & lines,
	std::unordered_map<std::string, std::vector<AssemblyInstruction>> & instructions,
	std::unordered_map<std::string, Directive>& directives) {
	debug("=== Stage 1: Parser.run() ===");

	//	1.	Open the input file
	std::ifstream input_file;

	input_file.open(config.input_file_name, std::ifstream::in);

	if (!input_file.is_open()) {
		throwError("Could not open file '" + config.input_file_name
			+ "' for reading.");
	}

	debug("file is opened for reading");

	//	2.	Generate a std::vector<std::string> line_strings vector from the
	//		input file.
	std::vector<std::string> line_strings;

	std::string currentString;
	while (input_file.good()) {
		std::getline(input_file, currentString);

		line_strings.push_back(currentString);
	}

	input_file.close();

	debug("file is closed");

	for (int i = 0; i < line_strings.size(); i++) {
		debug("Line " + std::to_string(i + 1) + ": " + line_strings[i]);
	}

	//	3.	Populate the Assembler's std::vector<Line *> lines vector

	//	The Parser assumes that the address of the first instruction will be 0.
	//	This value may end up being shifted by the Preprocessor if the user
	//	specified that the address of the first instruction should be 
	//	elsewhere.
	uint16_t current_address = 0;

	for (unsigned int line_count = 0; line_count < line_strings.size(); line_count++) {
		std::string s = line_strings[line_count];
		debug("== CONVERTING s to Line * ==");
		debug("s = " + s);

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

		debug("s' = " + line_string);

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
			debug("Token string: '" + s + "'");
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
			debug("Token: " + TokenToString(t));
		}

		//	3.4	Generate a Line object and add it to the vector<Line *> lines.
		Line* line = generateLine(line_count + 1, s, tokens, instructions, directives,
			current_address);

		//	Print line found

		debug("Line: " + LineToString(line));

		//	Check that line isn't invalid
		if (line->type == LineType::INVALID) {
			throwError("Line " + std::to_string(line_count + 1) 
				+ " is invalid.");
		}

		lines.push_back(line);
	}
}

Token Parser::stringToToken(std::string w, 
	std::unordered_map<std::string, std::vector<AssemblyInstruction>> & instructions) {
	//	Given a string w, match it with a TokenType and return a Token 
	//	containing w and the matched TokenType.

	TokenType type;

	if (isTEXTString(w)) {
		if (instructions.find(w) != instructions.end()) {
			//	Check whether w is an instruction (need Assembler's
			//	std::string -> std::vector<Instruction> hashmap to exist)
			type = TokenType::INSTRUCTION;
		}
		else if (general_register_bits.find(w) != general_register_bits.end()) {
			//	Check whether w is a general purpose register
			type = TokenType::GREGISTER;
		}
		else if (isOUT_PRINT_TYPEString(w)) {
			//	w is an OUT_PRINT_TYPE string
			type = TokenType::OUT_PRINT_TYPE;
		}
		else {
			//	Otherwise, w is TEXT
			type = TokenType::TEXT;
		}
	}
	else if (w.length() >= 2 && w[0] == ADDRESS_REGISTER_PREFIX &&
			special_register_bits.find(w.substr(1)) != special_register_bits.end()) {
		//	TODO: Remove this when LSP is deprecated.
		//	NOTE: This may cause bugs if any special register names conflict
		//	with address register names. However, special registers and address
		//	registers are not meant to coexist for long since address registers
		//	replace special registers. Hence, all special register logic should
		//	be removed when LSP is removed (relevant sections in the code that
		//	mention special registers are marked with TODOs).
		//	w is a special register
		type = TokenType::SREGISTER;
	}
	else if (w.length() >= 2 && w[0] == ADDRESS_REGISTER_PREFIX &&
		address_register_bits.find(w.substr(1)) != address_register_bits.end()) {
		//	w is an address register
		type = TokenType::AREGISTER;
	}
	else if (isNUMBERString(w)) {
		//	w is a number
		type = TokenType::NUMBER;
	}
	else if (isImmediateString(w)) {
		//	w is an immediate
		type = TokenType::IMMEDIATE;
	}
	else if (isDirectiveString(w)) {
		//	w appears to be a preprocessor directive (mark it as such - whether
		//	the directive exists will be checked in generateLine())
		type = TokenType::DIRECTIVE;
	}
	else if (isLABELString(w)) {
		//	w is a label
		type = TokenType::LABEL;
	}
	else if (isBYTE_CONSTANTString(w)) {
		//	w is a byte selection of a constant
		type = TokenType::BYTE_CONSTANT;
	}
	else if (isBYTE_LABELString(w)) {
		//	w is a byte selection of a label
		type = TokenType::BYTE_LABEL;
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
	case TokenType::OUT_PRINT_TYPE:
		return "OUT_PRINT_TYPE";
	case TokenType::TEXT:
		return "TEXT";
	//	TODO: REMOVE THIS WHEN LSP IS REMOVED.
	case TokenType::SREGISTER:
		return "SREGISTER";
	case TokenType::AREGISTER:
		return "AREGISTER";
	case TokenType::NUMBER:
		return "NUMBER";
	case TokenType::IMMEDIATE:
		return "IMMEDIATE";
	case TokenType::DIRECTIVE:
		return "DIRECTIVE";
	case TokenType::LABEL:
		return "LABEL";
	case TokenType::BYTE_CONSTANT:
		return "BYTE_CONSTANT";
	case TokenType::BYTE_LABEL:
		return "BYTE_LABEL";
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

Line* Parser::generateLine(unsigned int line_number, std::string s, 
	std::vector<Token> tokens,
	std::unordered_map<std::string, std::vector<AssemblyInstruction>> & instructions,
	std::unordered_map<std::string, Directive> & directives,
	uint16_t & current_address) {
	//	Given vector<Token> tokens, allocate and return a Line object.
	Line* line = nullptr;

	//	Identify line type
	if (tokens.size() == 0) {
		//	Line has no tokens - type is Empty
		return new Line(line_number, LineType::EMPTY, s, tokens, current_address);
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
				//	Update next line's address
				uint16_t instruction_address = current_address;
				current_address = getNextLineAddress(current_address, instruction);
				return new InstructionLine(line_number, s, tokens, instruction_address, &instruction);
			}
		}

		//	No valid instruction found
		throwErrorLine(line_number, "Incorrect arguments for instruction '"
			+ tokens[0].token_string + "'.");
	}
	if (tokens[0].type == TokenType::DIRECTIVE) {
		//	Line may be a Preprocessor directive - verify that the name of the
		//	directive matches a directive that exists, and that the rest of the
		//	tokens match the expected argument sequence of the directive found.
		std::string directiveName = tokens[0].token_string.substr(1);
		if (directives.find(directiveName) == directives.end()) {
			//	No directive with the given name found - throw an error
			throwErrorLine(line_number, "Unknown directive '" 
				+ directiveName + "'.");
		}

		Directive& directive = directives[directiveName];

		//	Ensure that rest of tokens match directive
		if (matchesDirectiveArgs(tokens, directive)) {
			return new DirectiveLine(line_number, s, tokens, current_address, &directive);
		}

		//	Directive exists, but given token arguments do not match its 
		//	expected arguments.
		throwErrorLine(line_number, 
			"Incorrect arguments for preprocessor directive '" + directive.name 
			+ "'.");
	}
	else if (tokens[0].type == TokenType::LABEL && tokens.size() == 1) {
		//	Line is a label definition.
		return new Line(line_number, LineType::LABEL_DEFINITION, s, tokens, current_address);
	}
	else {
		//	Invalid line
		return new Line(line_number, LineType::INVALID, s, tokens, current_address);
	}
}

bool Parser::matchesInstructionArgs(std::vector<Token> tokens, AssemblyInstruction& instruction) {
	//	Check whether token types match the instruction's expected arguments
	//	For this to be true:
	//	1.	tokens.size() - 1 == instruction.expected_param_types.size().
	//	2.	tokens[0].type == TokenType::INSTRUCTION.
	//	3.	for i = 0 to instruction.expected_param_types.size():
	//			tokens[i + 1].type matches expected_param_type[i]

	if (tokens.size() - 1 != instruction.expected_param_types.size())
		return false;

	if (tokens[0].type != TokenType::INSTRUCTION)
		return false;

	//	"Quantity" type arguments may be immediates, Labels, or constants
	//	(which are TEXT tokens that the parser will assume are the name of 
	//	preprocessor constants - if it isn't, then the preprocessor will throw
	//	an error).
	/*std::unordered_set<TokenType> quantity_types({ TokenType::IMMEDIATE,
		TokenType::LABEL, TokenType::TEXT });*/
	std::unordered_set<TokenType> quantity_types({
			TokenType::IMMEDIATE,
			TokenType::TEXT,
			TokenType::BYTE_CONSTANT,
			TokenType::BYTE_LABEL
		});

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

bool Parser::matchesDirectiveArgs(std::vector<Token> tokens, Directive& directive) {
	//	Check whether the TokenTypes of the directive tokens match the
	//	directive's expected TokenTypes argument sequence.
	//	For this to be true:
	//	1.	tokens.size() - 1 == directive.expected_param_types.size().
	//	2.	tokens[0].type == TokenType::DIRECTIVE.
	//	3.	for i = 0 to directive.expected_param_types.size():
	//			tokens[i + 1].type matches expected_param_type[i]

	if (tokens.size() - 1 != directive.expected_param_types.size())
		return false;

	if (tokens[0].type != TokenType::DIRECTIVE)
		return false;

	//	Note: No notion of "quantity types" for preprocessor directive 
	//	arguments, as these arguments will not be replaced by something else
	//	(such as the preprocessor or the label handler!)
	for (int i = 0; i < directive.expected_param_types.size(); i++) {
		if (tokens[i + 1].type != directive.expected_param_types[i])
			return false;
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
	std::stringstream lineAddressStringStream;
	lineAddressStringStream << std::setfill('0') << std::setw(4) << std::hex << line->line_address;
	return "{ address: 0x" + lineAddressStringStream.str() + ", type: " + LineTypeToString(line->type) + " }";
}

uint16_t Parser::getNextLineAddress(uint16_t current_address, AssemblyInstruction & instruction) {
	//	Compute address of next line based on size of AssemblyInstruction on
	//	current line.
	//	Size of assembly instruction = 
	//		length of derived ISAInstruction sequence * sizeof(ISAInstruction)
	uint16_t next_address = current_address
		+ (instruction.instruction_sequence.size() * ISA_INSTRUCTION_SIZE_BYTES);

	//	Check for overflow - if overflow occurs, the program is too large for
	//	the address space
	//	TODO - at this point, the Parser doesn't know whether the address is for
	//	a ROM program or a RAM program. If this is specified in a preprocessor
	//	directive, then the preprocessor needs to check that the program can
	//	fit in the ROM / RAM as well.
	//	However, the parser can still check that the program doesn't exceed the
	//	maximum possible length of a program, which right now is the size of the
	//	RAM.
	
	//	Address of last byte of the given instruction (only need to check for
	//	overflow on the given instruction, as it may be the last instruction in
	//	the program)
	uint16_t address_of_last_byte = next_address - 1;

	//	if last byte address < current_address, classic overflow occurred
	//	if last byte address >= RAM_SIZE_BYTES, then program is too big to be
	//	stored in the RAM (and therefore also too big for the ROM).
	if (address_of_last_byte < current_address
		|| address_of_last_byte > RAM_SIZE_BYTES) {
		throwError("Program is too big to store in either the RAM or the ROM.");
	}

	return next_address;
}