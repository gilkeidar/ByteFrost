#include "utility.hpp"
#include "constants.hpp"
#include <iostream>
#include <string>
#include <math.h>
#include <sstream>
#include <stdlib.h>


bool isAlpha(char c) {
	bool result = ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
	std::cout << "isAlpha(" << c << "):" << result << std::endl;
	return result;
}

bool isDigit(char c) {
	bool result = ('0' <= c && c <= '9');
	std::cout << "isDigit(" << c << "):" << result << std::endl;
	return result;
}

bool isHexDigit(char c) {
	bool result = isDigit(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
	std::cout << "isHexDigit(" << c << "):" << result << std::endl;
	return result;
}


bool isTEXTString(std::string s) {
	//	In order for s to be in TEXT:
	//	1. s must have length >= 1.
	//	2. s[0] must be in A or be an underscore.
	//	3. For all i s.t. 1 <= i < s.length(), s[i] must be in A, D, or an
	//		underscore.
	if (s.length() == 0)	return false;
	
	if (s[0] != UNDERSCORE && !isAlpha(s[0]))	return false;

	for (int i = 1; i < s.length(); i++) {
		if (s[i] != UNDERSCORE && !isAlpha(s[i]) && !isDigit(s[i]))
			return false;
	}

	return true;
}

bool isUnsignedDecimalString(std::string s) {
	std::cout << "isUnsignedDecimalString(" << s << ")" << std::endl;
	//	In order for s to be in ND:
	//	1. s must have length >= 1.
	//	2. (s[0] == '0' AND s.length() == 1)
	//		OR (s[0] != '0' AND for all i s.t. 0 <= i < s.length(), s[i] must
	//			be in D).
	if (s.length() == 0)	return false;
	if (s == "0")			return true;
	if (s[0] != '0') {
		for (int i = 0; i < s.length(); i++)
		{
			if (!isDigit(s[i]))
				return false;
		}
		return true;
	}

	return false;
}

bool isUnsignedHexadecimalString(std::string s) {
	std::cout << "isUnsignedHexadecimalString(" << s << ")" << std::endl;
	//	In order for s to be in NH:
	//	1. s must have length >= 3 (as every number must at least have "0x" as
	//		a prefix, and the number cannot be empty)
	//	2. s[0] = '0' AND s[1] = 'x' AND for all i s.t. 2 <= i < s.length(),
	//		s[i] must be in H).
	if (s.length() <= 2)			return false;
	if (s[0] != '0' || s[1] != 'x')	return false;
	for (int i = 2; i < s.length(); i++) {
		if (!isHexDigit(s[i]))
			return false;
	}

	return true;
}

bool isUnsignedNumberString(std::string s) {
	std::cout << "isUnsignedNumberString(" << s << ")" << std::endl;
	//	In order for s to be in N:
	//	1. s must be in ND or in NH.
	return isUnsignedDecimalString(s) || isUnsignedHexadecimalString(s);
}

bool isNUMBERString(std::string s) {
	std::cout << "isNUMBERString(" << s << ")" << std::endl;
	//	In order for s to be in NUMBER:
	//	1. s is not empty (s.length >= 1)
	//	2. (s[0] is '-' or '+' and s[1:end] is in N) || (s is in N)
	if (s.length() == 0)	return false;
	if (s[0] == '-' || s[0] == '+')	return isUnsignedNumberString(s.substr(1));
	return isUnsignedNumberString(s);
}

bool isImmediateString(std::string s) {
	//	In order for s to represent an immediate:
	//	1. s.length() >= 2.
	//	2. s[0] is IMMEDIATE_PREFIX
	//	3. s[1:end] is in NUMBER.
	if (s.length() < 2)	return false;
	if (s[0] != IMMEDIATE_PREFIX)	return false;

	return isNUMBERString(s.substr(1));
}

bool isDirectiveString(std::string s) {
	//	In order for s to represent a preprocessor directive invocation:
	//	1.	s.length() >= 2.
	//	2.	s[0] is DIRECTIVE_PREFIX
	//	3.	s[1:end] is in TEXT.
	if (s.length() < 2)	return false;
	if (s[0] != DIRECTIVE_PREFIX)	return false;

	return isTEXTString(s.substr(1));
}

bool isFILEString(std::string s) {
	std::cout << "isFILEString(" << s << ")" << std::endl;
	//	In order for s to be in FILE:
	//	1.	s may not be empty.
	//	2.	There exist strings u, v s.t. s = u.v, where u,v are in TEXT

	if (s.length() == 0)	return false;

	size_t dotIndex = s.find_first_of('.');

	std::cout << "dot index: " << std::to_string(dotIndex) << std::endl;
	
	//	No dot character in s
	if (dotIndex == s.npos)	return false;

	std::cout << "u = " << s.substr(0, dotIndex) << std::endl;
	std::cout << "v = " << s.substr(dotIndex + 1) << std::endl;

	return isTEXTString(s.substr(0, dotIndex))
		&& isTEXTString(s.substr(dotIndex + 1));
}

bool isLABELString(std::string s) {
	//	In order for s to represent a label:
	//	1.	s.length() >= 2.
	//	2.	s[0] is ':'.
	//	3.	s[1:end] is in TEXT.

	if (s.length() < 2)			return false;
	if (s[0] != LABEL_PREFIX)		return false;
	if (!isTEXTString(s.substr(1)))	return false;

	return true;
}

bool isBYTE_SELECTString(std::string s) {
	//	In order for s to be in BYTE_SELECT:
	//	1.	s.length() >= 3.
	//	2.	s[0] is '[' and s[s.length() - 1] is ']'.
	//	3.	s[1:s.length() - 2] is in ND.

	if (s.length() < 3)		return false;
	if (s[0] != BYTE_SELECT_START || s[s.length() - 1] != BYTE_SELECT_END)	
		return false;

	return isUnsignedDecimalString(s.substr(1, s.length() - 2));
}

bool isBYTE_CONSTANTString(std::string s) {
	//	In order for s to be in BYTE_CONSTANT:
	//	1.	s.length() >= 2.
	//	2.	s = uv such that u is in TEXT and v is in BYTE_SELECT.

	if (s.length() < 2)	return false;

	int byteSelectIndex = s.find_first_of(BYTE_SELECT_START);

	//	No byte select start character in s
	if (byteSelectIndex == s.npos)	return false;

	return isTEXTString(s.substr(0, byteSelectIndex))
		&& isBYTE_SELECTString(s.substr(byteSelectIndex));
}

bool isBYTE_LABELString(std::string s) {
	//	In order for s to be in BYTE_LABEL:
	//	1. s.length() >= 2.
	//	2. s = uv such that u is a label and v is in BYTE_SELECT.

	if (s.length() < 2)	return false;

	int byteSelectIndex = s.find_first_of(BYTE_SELECT_START);

	//	No byte select start character in s
	if (byteSelectIndex == s.npos)	return false;

	return isLABELString(s.substr(0, byteSelectIndex))
		&& isBYTE_SELECTString(s.substr(byteSelectIndex));
}

//	String parsing

bool stringEndsWith(std::string s, std::string ending) {
	//	s ends with the string ending iff there exists a string v s.t.
	//		s = v ending.
	//	Hence, s ends with string ending iff:
	//	1.	s.length() >= ending.length()
	//	2.	s[s.length() - ending.length()]...s[s.length() - 1] == ending
	if (s.length() < ending.length())	return false;
	
	for (int i = 0; i < ending.length(); i++)
	{
		if (s[s.length() - ending.length() + i] != ending[i])
			return false;
	}

	return true;
}

std::string removeFileExtension(std::string file_name) {
	if (!isFILEString(file_name)) {
		throwError("String '" + file_name + "' is not a valid file name string.");
	}
	int dotIndex = file_name.find_first_of('.');

	return file_name.substr(0, dotIndex);
}

std::string getFlagName(std::string flag_string) {
	if (flag_string.length() == 0 || flag_string[0] != '-') {
		throwError(flag_string + " is not a valid flag string (parsing error).");
	}
	return flag_string.substr(1);
}

//	Extracting value from string

long long getNumberValue(std::string number_string) {
	//	First, check that the given string represents a number. If it does not,
	//	throw an error.
	if (!isNUMBERString(number_string)) {
		throwError("Given string '" + number_string 
			+ "' does not represent a number.");
	}

	//	The given string is in NUMBER.
	//	NUMBER = {'', '-', '+'} \circ N
	//		   = {'', '-', '+'} \circ (ND U NH)
	std::string v = number_string;
	int multiplier = 1;

	if (!isUnsignedNumberString(v)) {
		//	v is not in N, so v[0] is '-' or '+', meaning v[1:end] is in N.
		if (v[0] == '-') {
			multiplier = -1;
		}

		//	Remove sign character from v
		v = v.substr(1);
	}

	//	v is in N.
	//	There are two cases:
	//	Case 1: v is in ND.
	//	Case 2: v is in NH.

	if (isUnsignedDecimalString(v)) {
		//	Case 1 - v is in ND.
		return multiplier * std::stoi(v);
	}
	else if (isUnsignedHexadecimalString(v)) {
		//	Case 2 - v is in NH.
		//std::stringstream converter;
		//long result;
		//converter << std::hex << v;
		/*converter << std::hex << "0xdeadbeef";
		converter >> result;
		std::cout << "number is in NH and has value " << std::hex << result << std::endl;
		return multiplier * result;*/
		long long result = strtoll(v.c_str(), nullptr, 16);
		std::cout << "sizeof(long): " << std::to_string(sizeof(long)) << std::endl;
		std::cout << "result: " << std::hex << result << std::endl;
		return multiplier * result;
	}
	else {
		//	Error - v is neither in ND or NH.
		throwError("String '" + v + "' is in N but not in NH or ND.");
	}
}

int getImmediateValue(std::string immediate_string) {
	//	First, check that the given string represents an immediate. If it does
	//	not, throw an error.
	if (!isImmediateString(immediate_string)) {
		throwError("Given string '" + immediate_string
			+ "' does not represent an immediate.");
	}

	//	An immediate is a string of the form '#'v where v is in NUMBER.
	//	NUMBER = {'', '-', '+'} \circ N
	//		   = {'', '-', '+'} \circ (ND U NH)

	//	Get the integer value of string v in NUMBER.
	return getNumberValue(immediate_string.substr(1));
}

std::string getConstantNameFromByteConstant(std::string byte_constant_string) {
	//	First, check that the given string is in BYTE_CONSTANT. If it is not,
	//	throw an error.
	if (!isBYTE_CONSTANTString(byte_constant_string)) {
		throwError("Given string '" + byte_constant_string 
			+ "' is not in BYTE_CONSTANT.");
	}

	//	In order for a string s to be in BYTE_CONSTANT:
	//	1.	s.length() >= 2.
	//	2.	s = uv such that u is in TEXT and v is in BYTE_SELECT.

	int byteSelectIndex = byte_constant_string.find_first_of(BYTE_SELECT_START);

	return byte_constant_string.substr(0, byteSelectIndex);
}

int getConstantIndexFromByteConstant(std::string byte_constant_string) {
	//	First, check that the given string is in BYTE_CONSTANT. If it is not,
	//	throw an error.
	if (!isBYTE_CONSTANTString(byte_constant_string)) {
		throwError("Given string '" + byte_constant_string
			+ "' is not in BYTE_CONSTANT.");
	}

	//	Get integer index string
	int byteSelectIndex = byte_constant_string.find_first_of(BYTE_SELECT_START);

	//	Index number string is a substring of byte_constant_string from index s
	//	to e, inclusive, defined as:
	//	s = index of BYTE_SELECT_START + 1
	//	e = index of BYTE_SELECT_END = byte_constant_string.size() - 2
	//	so length of number string is:
	//	e - s + 1 = byte_constant_string.size() - 2 - (byteSelectIndex + 1) + 1
	//		= byte_constant_size.size() - 2 - byteSelectIndex - 1 + 1
	//		= byte_constant_size.size() - byteSelectIndex - 2
	std::string indexString = byte_constant_string.substr(byteSelectIndex + 1,
		byte_constant_string.size() - byteSelectIndex - 2);

	return getNumberValue(indexString);
}

//	Extract value from integers

uint8_t getByteFromInt(long long integer, int byte) {
	//	Byte selection must be 0 - 7
	if (byte < 0 || byte >= sizeof(long long)) {
		throwError("Can only select bytes 0 - 7 from an long long; received byte selection "
			+ std::to_string(byte) + " instead.");
	}

	uint8_t byte_mask = 0xFF;

	return (integer >> (byte * 8)) & byte_mask;
}

//	String generation

std::string generateImmediateString(long immediate_value) {
	return IMMEDIATE_PREFIX + std::to_string(immediate_value);
}

//	Size Checking

//bool fitsInBitWidth(int x, int max_bit_width, bool unsigned_representation) {
//	//	Determine value's necessary (minimum) bit width when representing in
//	//	binary
//	//	Note: Given an integer value x, the minimum number of bits needed to
//	//		represent the value in a binary string is given as follows:
//	//	(Note: if x < 0, then the binary string is in 2's complement notation;
//	//			otherwise, it's unsigned)
//	//	Note that overrides previous note in parentheses: unsigned / two's
//	//	complement notation is decided by unsigned_representation bool input
//	//	TODO: FILL THIS METHOD
//	//	Given an integer value x, the minimum number of bits needed to
//	//	represent the value in a binary string (n) is given as follows:
//	//	Case 1: Representation is unsigned.
//	//		Case 1.1:	x = 0.
//	//			Then, n = 1.
//	//		Case 1.2:	x > 0.
//	//			Then, n = floor(log_2(x)) + 1.
//	//		Case 1.3:	x < 0.
//	//			Then n cannot be defined as representation is unsigned.
//	//			Throw an error.
//	//	Case 2:	Representation is signed 2's complement.
//	//		Case 2.1:	x = 0 or x = -1.
//	//			Then, n = 1.
//	//		Case 2.2:	x < -1.
//	//			Then, n = floor(log_2(|x| - 1)) + 2.
//	//		Case 2.3:	x >= 1.
//	//			Then, n = floor(log_2(x)) + 2.
//
//	//	minimum representation length
//	int n = INT_MAX;
//	
//	if (unsigned_representation) {
//		//	Case 1:	Representation is unsigned.
//		if (x == 0) {
//			//	Case 1.1.	x = 0.
//			n = 1;
//		}
//		else if (x > 0) {
//			//	Case 1.2.	x > 0.
//			//	n = floor(log_2(x)) + 1.
//			n = floor(log2(x)) + 1;
//		}
//		else {
//			//	Case 1.3. x < 0.
//			//	Impossible to represent in unsigned notation.
//			throwError("It is impossible to represent the negative value "
//				+ std::to_string(x) + " in an unsigned binary string of length "
//				+ std::to_string(max_bit_width) + ".");
//		}
//	}
//	else {
//		//	Case 2: Representation is signed 2's complement.
//		if (x == 0 || x == -1) {
//			//	Case 2.1:	x = 0 or x = -1.
//			n = 1;
//		}
//	}
//
//	return n <= max_bit_width;
//}
//
//bool fitsInBitWidth(int unsigned_value, int max_bit_width) {
//	//	Determines whether the given value may be represented in a binary
//	//	string of length max_bit_width.
//	//	NOTE: This function computes the minimum necessary length 
//	return false;
//}

bool fitsArgumentRange(int imm_value, int argument_size, ArgumentRepresentation argument_rep) {
	//	Let x be the immediate value and n be the argument size (bit width).
	//	A signed (2's complement) binary string of length n may represent
	//		values in the range -2^(n - 1) to 2^(n - 1) - 1.
	//	An unsigned binary string of length n may represent values in the range
	//		0 to 2^n - 1.
	//	Case 1:	argument_rep = ArgumentRepresentation::Signed
	//		Return true if x is within the range -2^(n - 1) to 2^(n - 1) - 1.
	//	Case 2: argument_rep = ArgumentRepresentation::Unsigned
	//		Return true if x is within the range 0 to 2^n - 1.
	//	Case 3: argument_rep = ArgumentRepresentation::SignedOrUnsigned
	//		Return true if x is within the range -2^(n - 1) to 2^n - 1.

	std::cout << "fitsArgumentRange(value: " << std::to_string(imm_value)
		<< " | n = " << std::to_string(argument_size) << " | rep: " 
		<< ArgumentRepresentationToString(argument_rep) << std::endl;

	int min_val_signed = (-1) << (argument_size - 1);		//	-2^(n - 1)
	int max_val_signed = (1 << (argument_size - 1)) - 1;	//	2^(n - 1) - 1
	int min_val_unsigned = 0;
	int max_val_unsigned = (1 << argument_size) - 1;		//	2^n - 1

	switch (argument_rep) {
		case ArgumentRepresentation::Signed:
			return (min_val_signed <= imm_value) && (imm_value <= max_val_signed);
		case ArgumentRepresentation::Unsigned:
			return (min_val_unsigned <= imm_value) && (imm_value <= max_val_unsigned);
		case ArgumentRepresentation::SignedOrUnsigned:
			return (min_val_signed <= imm_value) && (imm_value <= max_val_unsigned);
		default:
			throwError("Undefined default cause reached in fitsArgumentRange()");
	}
}

//	Error Handling

void throwError(std::string error_string) {
	std::cerr << "Error: " << error_string << std::endl;
	exit(1);
}

void throwErrorLine(unsigned int line_number, std::string error_string) {
	std::cerr << "Error: Line " << std::to_string(line_number) << ": " << error_string << std::endl;
	exit(1);
}

void throwWarning(std::string warning_string) {
	std::cerr << "Warning: " << warning_string << std::endl;
}

void throwWarningLine(unsigned int line_number, std::string warning_string) {
	std::cerr << "Warning: Line " << std::to_string(line_number) << ": " << warning_string << std::endl;
}