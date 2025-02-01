#pragma once
//#include "parser.hpp"
#include "shared_types.hpp"
#include "clap.hpp"

class OutputFileGenerator {
public:
	OutputFileGenerator() {}
	void run(std::vector<Line*> & lines, CommandLineArguments& clArgs);
private:
	std::vector<Line*> lines;

	void createMLGFile(std::ofstream& output_file);
	void createBINFile(std::ofstream& output_file);
};