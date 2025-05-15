#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "shared_types.hpp"
#include "clap.hpp"

struct Label {
	std::string name;

	/**
	 * @brief The normal 16-bit address the Label points to.
	 * @note This is NOT the PC value at this address!
	 */
	uint16_t address;

	/**
	 * @brief Gets the PC address value of this Label.
	 * @return The PC Address value of this Label (i.e., the address field
	 * logically shifted right by 1).
	 */
	uint16_t getPCAddress();
};

class LabelHandler {
public:
	LabelHandler(Config & config);
	void run(std::vector<Line*>& lines);
private:
	/**
	 * @brief Reference to the Assembler's Config instance.
	 */
	Config& config;

	/**
	 * @brief Hashmap of string (label name) -> Label, populated in the first
	 * pass through the lines vector in LabelHandler.run();
	 */
	std::unordered_map<std::string, Label> labels;
};