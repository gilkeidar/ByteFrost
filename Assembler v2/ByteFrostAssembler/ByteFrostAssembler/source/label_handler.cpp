#include "label_handler.hpp"
#include "utility.hpp"
#include <iostream>
#include <unordered_set>

uint16_t Label::getPCAddress() {
	//	NOTE - this assumes that the C++ compiler will implement this as a
	//	logical shift right. Since the shifted type is unsigned though, this is
	//	precisely what should occur.
	throwWarning("getPCAddress() - ensure this implements logical shift right!");;
	return this->address >> 1;
}

LabelHandler::LabelHandler() {

}

void LabelHandler::run(std::vector<Line*>& lines, 
	CommandLineArguments& args) {
	std::cout << "=== Stage 3: LabelHandler.run() ===" << std::endl;

	//	First pass - populate the string -> Label labels hashmap in the 
	//	LabelHandler
	for (Line* line : lines) {
		//	If the current line is a Label Declaration line, attempt to add the
		//	new Label to the labels hashmap.
		//	If a Label with this label name already exists, throw an error.
		if (line->type == LineType::LABEL_DEFINITION) {
			//	Create new Label
			Label newLabel {
				getLabelName(line->tokens[0].token_string),
				line->line_address
			};

			//	If label name already exists in labels hashmap, throw an error
			if (this->labels.find(newLabel.name) != this->labels.end()) {
				throwErrorLine(line->number, "Duplicate definition of label '"
					+ newLabel.name + "'.");
			}

			//	Add Lavbel to labels hashmap
			this->labels[newLabel.name] = newLabel;
		}
	}

	//	Second pass - replace all label tokens (LABEL, BYTE_LABEL) with
	//	immediates containing their values. If a label token is encountered
	//	with an unrecognized label name, throw an error.

	//	Define label token types; all tokens with these types in 
	//	InstructionLines will be replaced with an Immediate token (or an error
	//	will be thrown if an unknown label is encountered).
	std::unordered_set<TokenType> label_types({
			TokenType::BYTE_LABEL, TokenType::LABEL
		});

	for (Line* line : lines) {
		//	If current line is an InstructionLine, then iterate through its
		//	tokens vector and replace tokens of types in the label_types set
		//	with an immediate; the value of the immediate comes from the Label
		//	with the matching label name in the labels hashmap.
		if (line->type == LineType::INSTRUCTION) {
			for (Token & token : line->tokens) {
				if (label_types.find(token.type) == label_types.end())
					//	Token doesn't have a label token type
					continue;

				uint16_t immediate_value;
				if (token.type == TokenType::BYTE_LABEL) {
					//	1.	Get label name and byte index from token string.
					std::string labelName = getLabelNameFromByteLabel(token.token_string);
					int labelIndex = getLabelIndexFromByteLabel(token.token_string);
					
					//	2.	Attempt to find label in labels hashmap
					if (this->labels.find(labelName) == this->labels.end()) {
						//	Label not found - throw error
						throwErrorLine(line->number, "Undefined label '"
							+ labelName + "'.");
					}
					Label& label = this->labels[labelName];

					//	3.	Ensure that the index used can fit the label's size
					if (labelIndex < 0 || labelIndex >= LABEL_SIZE_BYTES) {
						throwErrorLine(line->number, "Index "
							+ std::to_string(labelIndex)
							+ " is out of bounds for a label, which is of size "
							+ std::to_string(LABEL_SIZE_BYTES) + " bytes.");
					}

					//	4.	Get addresss value from label and index
					//		NOTE - the value here will be from the normal
					//				address, NOT the PC address!
					immediate_value = getByteFromInt(label.address, labelIndex);
				}
				else if (token.type == TokenType::LABEL) {
					//	1.	Attempt to find label in LabelHandler's
					//		string -> Label hashmap labels
					std::string labelName = getLabelName(token.token_string);

					if (this->labels.find(labelName) == this->labels.end()) {
						//	Label not found - throw error
						throwErrorLine(line->number, "Undefined label '"
							+ labelName + "'.");
					}
					Label& label = this->labels[labelName];

					//	2.	Get immediate value from label
					//		NOTE - the value here IS THE PC ADDRESS OF THE
					//				LABEL!
					immediate_value = label.getPCAddress();
				}

				//	Replace token type with TokenType::IMMEDIATE and value with
				//	immediate string
				token.type = TokenType::IMMEDIATE;
				token.token_string = generateImmediateString(immediate_value);
			}
		}
	}
}