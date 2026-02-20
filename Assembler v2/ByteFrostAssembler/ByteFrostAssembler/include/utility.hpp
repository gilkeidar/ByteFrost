#pragma once
#include <string>
#include "constants.hpp"
#include "isa.hpp"
#include "shared_types.hpp"

/*	Debug Mode	*/

/**
 * @brief Prints the given string to stdout if the DEBUG_PRINTOUTS constant is
 * set to true in constants.hpp.
 * @param debug_printout debug string to print to stdout.
 */
void debug(std::string debug_printout);

/*	Parsing Utilities	*/

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

/**
 * @brief Determines whether a given string is an OUT print type string
 * (i.e., whether s is ASCII_PRINT_TYPE or INTEGER_PRINT_TYPE).
 * @param s string to check
 * @return true if s is an OUT_PRINT_TYPE string and false otherwise.
 */
bool isOUT_PRINT_TYPEString(std::string s);

/**
 * @brief Determines whether a given string is an AR (H/L) string (i.e.,
 * whether s is ARHorL_LOW or ARHorL_HIGH).
 * @param s string to check
 * @return true if s is an ARHorL string and false otherwise.
 */
bool isARHorLString(std::string s);

/**
 * @brief Determines whether a given string is a character string (i.e., a
 * string of the form 'x' where x is a single character or a string beginning
 * with \ (for escape characters)).
 * @param s string to check
 * @return true if s is a Character string and false otherwise.
 */
bool isCharacterString(std::string s);

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
 * @brief Given a string that represents a character, this function returns the
 * represented character's ASCII value.
 * @param char_string string that represents a character
 * @return The ASCII value of the character represented by char_string.
 * @note This function throws an error if the given string does not represent a
 * character.
 */
int getCharValue(std::string char_string);

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

/*	Size Checking	*/

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

/*	Error Handling	*/

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

/*	Misc.	*/

/**
 * @brief Logical shifts the given integer x right by the specified shift
 * amount.
 * @param x integer to logically shift right
 * @param shift_amt number of bits to logically shift x by
 * @return x >> shift_amt, where >> is a logical shift right.
 */
int logicalShiftRight(int x, int shift_amt);
