#pragma once
#include <string>
#include <vector>
#include <unordered_map>

enum class CLTokenType {FLAG, NUMBER, FILE_NAME, TEXT, INVALID};

struct CLToken {
	CLTokenType type;
	std::string token_string;
};

struct CLFlag {
	std::string flag_name;
	std::vector<CLTokenType> expected_pattern;
	std::vector<CLToken> values;
	bool is_set = false;
};

/**
 * @brief Converts a given string to a CLToken.
 * @param s string to convert to a CLToken
 * @return CLToken matching the given string.
 */
CLToken stringToCLToken(std::string s);

std::string CLTokenTypeToString(CLTokenType type);

class CommandLineArguments {
public:
	/**
	 * @brief CommandLineArguments constructor; initializes flags hashmap
	 * and the input_file_name to UNSET_FILE_NAME.
	 */
	CommandLineArguments();
	std::string input_file_name;
	std::unordered_map<std::string, CLFlag> flags;
private:
};

/**
 * @brief Command-Line Argument Parser (CLAP) class, which parses the
 * Assembler's command-line arguments and updates its CommandLineArguments
 * object to correctly modify the assembly pipelines. Will throw an error due
 * to command-line argument misuse.
 */
class CLAP {
public:
	/**
	 * @brief CLAP constructor
	 * @param argc Number of command-line arguments
	 * @param argv Command-line argument string array
	 */
	CLAP(int argc, char ** argv);

	/**
	 * @brief Runs the CLAP on the given command-line arguments, which
	 *	generates a CommandLineArguments object.
	 * @return CommandLineArguments object pointer.
	 */
	CommandLineArguments * run();
private:
	/**
	 * @brief Number of command-line arguments (including assembler program
	 * name).
	 */
	int argc;

	/**
	 * @brief Command-line argument string array.
	 */
	char** argv;
};

