#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "shared_types.hpp"

/**
 * @brief CLAP-recognized CLToken types; a CLToken's token string is mapped to
 * one of these CLToken types by the CLAP.
 */
enum class CLTokenType {FLAG, NUMBER, FILE_NAME, TEXT, INVALID};

/**
 * @brief Struct containing a token string and its identified CLTokenType.
 */
struct CLToken {
	/**
	 * @brief The CLTokenType of this CLToken.
	 */
	CLTokenType type;

	/**
	 * @brief The (non-empty) token string of this CLToken. Token strings
	 * contain no delimiters (in the case of the CLAP, whitespace - but the
	 * CLAP does not determine the command-line token strings, the CLI tool 
	 * used to invoke the ByteFrost Assembler does this).
	 */
	std::string token_string;
};

/**
 * @brief This struct defines a Command-Line flag recognized by the ByteFrost
 * Assembler. CLTokens of type CLTokenType::FLAG are mapped by the CLAP to a
 * corresponding CFlag struct in the CommandLineArguments object.
 */
struct CLFlag {
	/**
	 * @brief Command-line flag name (not including the '-' flag prefix)
	 */
	std::string flag_name;
	
	/**
	 * @brief Flag's expected argument sequence (akin to a function prototype).
	 */
	std::vector<CLTokenType> expected_pattern;

	/**
	 * @brief Flag's argument sequence (filled by CLAP when given command-line
	 * arguments)
	 */
	std::vector<CLToken> values;

	/**
	 * @brief Whether this flag has been set; defaults to false.
	 */
	bool is_set = false;
};

/**
 * @brief Converts a given string to a CLToken.
 * @param s string to convert to a CLToken
 * @return CLToken matching the given string.
 */
CLToken stringToCLToken(std::string s);

/**
 * @brief Returns a string representation of the given CLTokenType.
 * @param type CLTokenType to represent as a string
 * @return std::string representation of the given CLTokenType.
 */
std::string CLTokenTypeToString(CLTokenType type);

class CommandLineArguments {
public:
	/**
	 * @brief CommandLineArguments constructor; initializes flags hashmap
	 * and the input_file_name to UNSET_FILE_NAME.
	 */
	CommandLineArguments();

	/**
	 * @brief Given a reference to a Config instance, this method sets the
	 * values of command-line settable properties based on the input_file_name
	 * string and all set flags.
	 * @param config Reference to a Config instance.
	 */
	void updateConfig(Config& config);

	/**
	 * @brief File name of the input .asm file (filled by CLAP).
	 * TODO: Replace std::string with std::optional<std::string> since the
	 * input_file_name may not be set; using a special value (UNSET_FILE_NAME)
	 * is a less clean way of checking this in my opinion.
	 */
	std::string input_file_name;

	/**
	 * @brief flag name (string) -> CFLag hashmap. This hashmap is initialized
	 * by the CommandLineArguments constructor to contain all CLAP-recognized
	 * command-line flags.
	 */
	std::unordered_map<std::string, CLFlag> flags;
private:
};

/**
 * @brief Command-Line Argument Parser (CLAP) class, which parses the
 * Assembler's command-line arguments and generates a CommandLineArguments
 * object to correctly modify the assembly pipeline. Will throw an error due
 * to command-line argument misuse.
 */
class CLAP {
public:
	/**
	 * @brief CLAP constructor
	 * @param argc Number of command-line arguments
	 * @param argv Command-line argument string array
	 * @param config Reference to a Config instance
	 */
	CLAP(int argc, char ** argv, Config & config) : argc(argc), argv(argv),
		config(config) {}

	/**
	 * @brief Runs the CLAP on the given command-line arguments, which
	 *	generates a CommandLineArguments object.
	 * @return CommandLineArguments object pointer.
	 */
	void run();
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

	/**
	 * @brief Reference to the Assembler's Config instance.
	 */
	Config& config;
};

