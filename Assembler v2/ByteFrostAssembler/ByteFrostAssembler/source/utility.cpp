#include "utility.hpp"
#include "constants.hpp"
#include <iostream>
#include <string>

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
	//	1. (s[0] is '-' or '+' and s[1:end] is in N) || (s is in N)
	if (s.length() == 0)	return false;
	if (s[0] == '-' || s[0] == '+')	return isUnsignedNumberString(s.substr(1));
	return isUnsignedNumberString(s);
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

//	Error Handling

void throwError(std::string error_string) {
	std::cerr << "Error: " << error_string << std::endl;
	exit(1);
}

void throwWarning(std::string warning_string) {
	std::cerr << "Warning: " << warning_string << std::endl;
}