#pragma once
#include <string>
#include <vector>
#include <unordered_map>

//	Forward declaration of Assembler class
class Assembler;

enum CLTokenType {FLAG, NUMBER, FILE_NAME, TEXT, INVALID};

struct CLToken {
	CLTokenType type;
	std::string token_string;
};

/**
 * @brief Converts a given string to a CLToken.
 * @param s string to convert to a CLToken
 * @return CLToken matching the given string.
 */
CLToken stringToCLToken(std::string s);

std::string CLTokenTypeToString(CLTokenType type);

struct CLFlag {
	std::string flag_name;
	std::vector<CLToken> expected_pattern;
	bool is_set = false;
};

class CommandLineArguments {
public:
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
	 * @brief CLAP assembler
	 * @param assembler reference to the Assembler class instance.
	 * @param argc Number of command-line arguments
	 * @param argv Command-line argument string array
	 */
	CLAP(Assembler * assembler, int argc, char ** argv);

	/**
	 * @brief Runs the CLAP on the given command-line arguments, updating the
	 * Assembler instance's CommandLineArguments object.
	 */
	void run();
private:
	/**
	 * @brief Pointer to the Assembler class instance.
	 */
	Assembler * assembler;

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

