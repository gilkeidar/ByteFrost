#pragma once
#include <string>

//	Parsing Utilities

//	Character classification

/**
 * @brief Determines whether a given character is a lowercase or uppercase
 * letter of the English alphabet. (i.e., whether c is in A).
 * @param c character to check
 * @return true if c is a lowercase or uppercase letter of the English alphabet
 * and false otherwise.
 */
bool isAlpha(char c);

/**
 * @brief Determines whether a given character is a digit (i.e., whether c is
 * in D).
 * @param c character to check
 * @return true if c is a digit and false otherwise.
 */
bool isDigit(char c);

/**
 * @brief Determines whether a given character is a hex digit (i.e., whether c
 * is in H).
 * @param c character to check
 * @return true if c is a hex digit and false otherwise.
 */
bool isHexDigit(char c);

//	String classification

/**
 * @brief Determines whether a given string is a member of the set TEXT.
 * (TEXT = (A U {_}) \circ (A U {_} U D)^*)
 * @param s string to check
 * @return true if s is in TEXT and false otherwise.
 */
bool isTEXTString(std::string s);

/**
 * @brief Determines whether a given string represents an unsigned decimal
 * number (i.e., whether s is in ND).
 * @param s string to check
 * @return true if s is in ND and false otherwise.
 */
bool isUnsignedDecimalString(std::string s);

/**
 * @brief Determines whether a given string represents an unsigned hexadecimal
 * number (i.e., where s is in NH).
 * @param s string to check
 * @return true if s is in NH and false otherwise.
 */
bool isUnsignedHexadecimalString(std::string s);

/**
 * @brief Determines whether a given string represents an unsigned number
 * (i.e., whether s is in N)
 * @param s string to check
 * @return true if s is in N and false otherwise.
 */
bool isUnsignedNumberString(std::string s);

/**
 * @brief Determines whether a given string represents a number (i.e., whether
 * s is in NUMBER).
 * (NUMBER = {'', '-', '+'} \circ N)
 * @param s string to check
 * @return true if s is in NUMBER and false otherwise.
 */
bool isNUMBERString(std::string s);

/**
 * @brief Determines whether a given string represents a file name (i.e.,
 * whether s is in FILE).
 * (FILE = TEXT \circ \{.\} \circ TEXT)
 * @param s string to check
 * @return true if s is in FILE and false otherwise.
 */
bool isFILEString(std::string s);

//	String parsing

/**
 * @brief Determines whether the string s ends with the string ending.
 * I.e., does there exist a string v s.t. s = v ending
 * @param s string to check
 * @param ending string that s may end with
 * @return true if s ends with ending and false otherwise.
 */
bool stringEndsWith(std::string s, std::string ending);

/**
 * @brief Given a file name string, return the name of the file without its
 * file extension. (e.g., given "test.asm", return "test")
 * @param file_name 
 * @return 
 */
std::string removeFileExtension(std::string file_name);

/**
 * @brief Returns the flag name of a given flag string.
 * @note This function throws an error if the flag_string does not begin with a
 * flag prefix ('-')
 * @param flag_string Flag string
 * @return flag name of the given flag string
 */
std::string getFlagName(std::string flag_string);

//	Error Handling

/**
 * @brief Prints the given error string to stderr, then exits the program with
 * exit code 1.
 * @param error_string Error string message
 */
void throwError(std::string error_string);

/**
 * @brief Prints the given warning string to stderr, but does not exit the
 * program.
 * @param warning_string Warning string massage
 */
void throwWarning(std::string warning_string);
