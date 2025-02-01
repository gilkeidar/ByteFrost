#   ISA Definition

The ByteFrost ISA is defined in the ByteFrost Assembler as a vector of
`ISAInstruction` structs, each of which defines one ByteFrost ISA instruction.
These structs have a function `generateCode()` that returns a `uint16_t` of
the instruction string containing any `Argument`s.

##  Arguments

There are two kinds of ISA instruction arguments - `ExpectedArgument`s and
`Argument`s, defined as follows:

```cpp
enum class ArgumentType {Rd, Rs1, Rs2, Immediate, func, ...};

struct ExpectedArgument {
    ArgumentType type;
    int size;     //  size of argument in bits
    int position; //  position of argument's starting bit in instruction string
}

struct Argument {
    ArgumentType type;
    int size;
    int value;
}
```

##  ISA Instructions

```cpp
struct ISAInstruction {
    int opcode;
    std::vector<ExpectedArgument> expected_arguments;

    uint16_t generateCode(std::vector<Argument> arguments);
}
```
