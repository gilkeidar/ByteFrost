#pragma once
#include <string>
#include "isa.hpp"
//#include "parser.hpp"
#include "shared_types.hpp"
//#include "preprocessor.hpp"

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
 * @brief Determines whether a given string represents an immediate.
 * @param s string to check
 * @return true if s represents an immediate and false otherwise.
 */
bool isImmediateString(std::string s);

/**
 * @brief Determines whether a given string represents a preprocessor directive
 * invocation.
 * @param s string to check
 * @return true if s represents a preprocessor directive invocation and false
 * otherwise.
 */
bool isDirectiveString(std::string s);

/**
 * @brief Determines whether a given string represents a file name (i.e.,
 * whether s is in FILE).
 * (FILE = TEXT \circ \{.\} \circ TEXT)
 * @param s string to check
 * @return true if s is in FILE and false otherwise.
 */
bool isFILEString(std::string s);

/**
 * @brief Determines whether a given string represents a label (i.e. whether
 * s is of the form : \circ TEXT)
 * @param s string to check
 * @return true if s represents a label and false otherwise.
 */
bool isLABELString(std::string s);

/**
 * @brief Determines whether a given string represents a byte selection (i.e.,
 * whether s is of the form [ ND ])
 * @param s string to check
 * @return true if s is in BYTE_SELECT and false otherwise.
 */
bool isBYTE_SELECTString(std::string s);

/**
 * @brief Determines whether a given string is a byte selection of a constant
 * (i.e., whether s is of the form TEXT \circ BYTE_SELECT)
 * @param s string to check
 * @return true if s is in BYTE_CONSTANT and false otherwise.
 */
bool isBYTE_CONSTANTString(std::string s);

/**
 * @brief Determines whether a given string is a byte selection of a label
 * (i.e., whether s is of the form LABEL \circ BYTE_SELECT)
 * @param s string to check
 * @return true if s is in BYTE_LABEL and false otherwise
 */
bool isBYTE_LABELString(std::string s);

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

//	Extracting value from string

/**
 * @brief Given a string that represents a number, this function returns the
 * number's value.
 * @param number_string string that represents a number.
 * @return long long (8 bytes) integer value of the given number string.
 * @note This function throws an error if the given string does not represent a
 * number.
 */
long long getNumberValue(std::string number_string);

/**
 * @brief Given a string that represents an immediate, this function returns
 * the immediate's value.
 * @param immediate_string string that represents an immediate.
 * @return integer value of the given immediate string.
 * @note This function throws an error if the given string does not represent
 * an immediate.
 */
int getImmediateValue(std::string immediate_string);

/**
 * @brief Given a string that is in BYTE_CONSTANT, this function returns the
 * constant's name.
 * @param byte_constant_string string that is in BYTE_CONSTANT
 * @return name of constant (substring of given string before byte selection
 * array syntax)
 * @note This function throws an error if the given string is not in 
 * BYTE_CONSTANT.
 */
std::string getConstantNameFromByteConstant(std::string byte_constant_string);

/**
 * @brief Given a string that is in BYTE_CONSTANT, this function returns the
 * integer index in the byte selection array syntax.
 * @param byte_constant_string string that is in BYTE_CONSTANT
 * @return integer index in the byte selection array syntax.
 * @note This function throws an error if the given string is not in 
 * BYTE_CONSTANT.
 */
int getConstantIndexFromByteConstant(std::string byte_constant_string);

/**
 * @brief Given a label string, this function returns the label name string.
 * @param label_string Label string from which to extract the label name.
 * @return Label name string from given label string.
 * @note This function throws an error if the given string is not a label
 * string.
 */
std::string getLabelName(std::string label_string);

/**
 * @brief Given a string that is in BYTE_LABEL, this function returns the 
 * label's name.
 * @param byte_label_string string that is in BYTE_LABEL
 * @return name of label (substring of given string before byte selection array
 * syntax)
 * @note This function throws an error if the given string is not in 
 * BYTE_LABEL.
 */
std::string getLabelNameFromByteLabel(std::string byte_label_string);

/**
 * @brief Given a string that is in BYTE_LABEL, this function returns the
 * integer index in the byte selection array syntax.
 * @param byte_label_string string that is in BYTE_LABEL
 * @return integer index in the byte selection array syntax.
 * @note This function throws an error if the given string is not in 
 * BYTE_LABEL.
 */
int getLabelIndexFromByteLabel(std::string byte_label_string);

//	Extract value from int

/**
 * @brief Returns an unsigned byte value of the specified byte from the given
 * long long.
 * @param integer long long integer to extract byte from
 * @param byte which byte to extract from long long (0 being the LSB and 7 being
 * the MSB)
 * @return specified unsigned byte value from given integer.
 * @note This function throws an error if the byte selected is not 0 - 7.
 */
uint8_t getByteFromInt(long long integer, int byte);

//	String generation

/**
 * @brief Generates an immediate string with the given value.
 * @param immediate_value immediate value
 * @return Immediate string with the given number value.
 */
std::string generateImmediateString(long immediate_value);

//	Size Checking

///**
// * @brief Determines whether the given integer value can be represented in a
// * binary string of the specified length (bit_width).
// * @param value integer value to check
// * @param bit_width length of binary string
// * @param unsigned_representation whether the binary string representation is
// * unsigned (true) or two's complement signed (false)
// * @return true if value can be represented in a binary string of the specified
// * length, and false otherwise.
// * @deprecated (not implemented properly - use unsigned-only version)
// */
//bool fitsInBitWidth(int value, int bit_width, bool unsigned_representation);
//
///**
// * @brief Determines whether the given integer value can be represented in a
// * binary string of the specified length (max_bit_width).
// * @param unsigned_value integer value to check (value is treated as unsigned).
// * @param max_bit_width length of binary string.
// * @note  If the unsigned_value integer is negative, this function will throw
// * a warning as the computation may be inaccurate.
// * @return true if the value can be represented in a binary string of the
// * specified length, and false otherwise.
// */
//bool fitsInBitWidth(int unsigned_value, int max_bit_width);

/**
 * @brief Determines whether the given integer value can be represented in a
 * binary string of the specified length (argument_size) given the 
 * ISAInstruction's ExpectedArgument's ArgumentRepresentation (which determines
 * the range of possible values in the binary string of the specified length).
 * @param imm_value Immediate value to check
 * @param argument_size length of binary string which needs to store the 
 *		immediate value
 * @param argument_rep the kind of representation (signed, unsigned, or either)
 * that the binary string can use.
 * @return true if the immediate value can fit in the binary string using the
 * specified representation and specified length, and false otherwise.
 */
bool fitsArgumentRange(int imm_value, int argument_size, ArgumentRepresentation argument_rep);

//	Error Handling

/**
 * @brief Prints the given error string to stderr, then exits the program with
 * exit code 1.
 * @param error_string Error string message
 */
void throwError(std::string error_string);

/**
 * @brief Prints the given error string to stderr, then exits the program with
 * exit code 1.
 * @param line_number Line number at which error was detected
 * @param error_string Error string message
 */
void throwErrorLine(unsigned int line_number, std::string error_string);

/**
 * @brief Prints the given warning string to stderr, but does not exit the
 * program.
 * @param warning_string Warning string massage
 */
void throwWarning(std::string warning_string);

/**
 * @brief Prints the given warning string to stderr, but does not exit the
 * program.
 * @param line_number Line number at which warning was detected.
 * @param warning_string Warning string message
 */
void throwWarningLine(unsigned int line_number, std::string warning_string);

