#include "isa.hpp"
#include "utility.hpp"

uint16_t ISAInstruction::generateCode(std::vector<Argument> arguments) const {
	//	Generate 16-bit instruction string of given ISA Instruction using given
	//	arguments
	
	//	Initially, instruction string is set to
	//	[00000000] [000 [5 bit opcode]] = opcode
	uint16_t instruction_string = this->opcode;

	//	Ensure that the number of received Arguments matches the number of
	//	ExpectedArguments
	if (this->expected_arguments.size() != arguments.size()) {
		throwError("Assembly Instruction Derivation: ISA instruction opcode "
			+ std::to_string(this->opcode) + " expects "
			+ std::to_string(this->expected_arguments.size())
			+ " arguments, but received "
			+ std::to_string(arguments.size()) + " arguments.");
	}

	//	Iterate through the given Arguments
	for (int i = 0; i < arguments.size(); i++) {
		//	Get the corresponding ExpectedArgument
		const ExpectedArgument& e = this->expected_arguments[i];

		//	Verify ExpectedArgument and Argument are of the same type
		if (e.type != arguments[i].type) {
			throwError("Assembly Instruction Derivation: ISA instruction opcode "
				+ std::to_string(this->opcode) + " expects argument "
				+ std::to_string(i + 1) + " to have type "
				+ ArgumentTypeToString(e.type)
				+ ", but received an argument with type "
				+ ArgumentTypeToString(arguments[i].type) + ".");
		}

		if (e.size != arguments[i].size) {
			//	Throw warning if incoming argument size is not equal to 
			//	expected argument size
			throwWarning("Assembly Instruction Derivation: ISA instruction opcode "
				+ std::to_string(this->opcode) + " expects argument"
				+ std::to_string(i + 1) + " to have size "
				+ std::to_string(e.size)
				+ ", but received an argument with size "
				+ std::to_string(arguments[i].size)
				+ ((e.size < arguments[i].size) ? " (POTENTIAL DATA LOSS) " : "")
				+ ".");
		}

		//	Add Argument to instruction string

		//	Compute size mask (e.g., if size = 4, size_mask = 0xF (1111))
		//	(if size = 2, size_mask = 0011, etc.)
		//	For size = 1, size_mask = (1 << 1) - 1 = 10 - 1 = 1
		//	For size = 2, size_mask = (1 << 2) - 1 = 100 - 1 = 11
		//	for size = 3, size_mask = (1 << 3) - 1 = 1000 - 1 = 111
		//	etc.

		//	TODO: When handling ARSrc for LDW, SDW, or MAA instructions, check
		//	the argument size (e.size) should be 1 for the top bit and the
		//	second bit should set the opcode's lsb.
		//	Not sure if ARSrc should have a size of 2 then, perhaps that would
		//	make the most sense and just don't use the size_mask implementation
		//	here (just do if opcode = LDW, SDW, or MAA then do ARSrc specific
		//	logic; otherwise, do normal logic for contiguous arguments (i.e.,
		//	whose values are contiguously represented in the instruction
		//	string).
		//	OR perhaps there should be two ARSrc arguments (ARSrc 1 and ARSrc
		//	2) since they have different placements?

		uint16_t size_mask = (1 << e.size) - 1;

		instruction_string |= (arguments[i].value & size_mask) << e.position;
	}

	return instruction_string;
}

std::string ArgumentRepresentationToString(ArgumentRepresentation r) {
	switch (r) {
		case ArgumentRepresentation::Signed:
			return "Signed";
		case ArgumentRepresentation::Unsigned:
			return "Unsigned";
		case ArgumentRepresentation::SignedOrUnsigned:
			return "SignedOrUnsigned";
		default:
			return "UNKNOWN_REPRESENTATION";
	}
}

std::string ArgumentTypeToString(ArgumentType t) {
	switch (t) {
		case ArgumentType::Rd:
			return "Rd";
		case ArgumentType::Rs1:
			return "Rs1";
		case ArgumentType::Rs2:
			return "RS2";
		case ArgumentType::Immediate:
			return "Immediate";
		case ArgumentType::Func:
			return "Func";
		default:
			return "UNKNOWN_TYPE";
	}
}