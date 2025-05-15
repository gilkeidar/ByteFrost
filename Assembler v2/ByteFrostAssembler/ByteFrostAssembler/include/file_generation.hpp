#pragma once
//#include "parser.hpp"
#include "shared_types.hpp"
#include "clap.hpp"

class OutputFileGenerator {
public:
	OutputFileGenerator(Config & config) : config(config) {}
	void run(std::vector<Line*> & lines);
private:
	/**
	 * @brief Reference to the Assembler's Config instance.
	 */
	Config& config;

	std::vector<Line*> lines;

	void createMLGFile(std::ofstream& output_file);
	void createBINFile(std::ofstream& output_file);
};