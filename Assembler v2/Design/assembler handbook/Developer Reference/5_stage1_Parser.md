#   Stage 1 - Parser

##  Overview

This is the second stage of the ByteFrost Assembler pipeline.

The **Parser** takes as input the `CommandLineArguments` object pointer
generated by the `CLAP`, the `Assembler`'s assembly instruction name 
(`std::string`) to `vector<AssemblyInstruction>` `instructions` hashmap, and the
`Assembler`'s Preprocessor directive name (`std::string`) to `Directive`
`directives` hashmap, and returns a `std::vector<Line *> lines` vector that
contains a `Line *` to a `Line` object for each line in the input `.asm` file
(assuming this file exists).

##  Definitions

### Delimiter Set `delimiters`

The delimiter set is an `std::unordered_set<char>` stored by the `Parser` class
that contains all characters that are understood to be delimiters in a line and
which should be used as separators between token strings. The delimiter set
`std::unordered_set<char> delimiters` is an implementation of the following set:

$DELIMITERS = \{\texttt{SPACE}, \texttt{TAB}, \texttt{,}\}$

Consequently, a token string recognized by the Parser contains none of the above
delimiter characters.

### `TokenType`

The Parser recognizes the following token (`Token`) types, which are the values
of the `TokenType` enum:

```cpp
enum TokenType = {INSTRUCTION, GREGISTER, OUT_PRINT_TYPE, TEXT, SREGISTER, 
NUMBER, IMMEDIATE, DIRECTIVE, LABEL, BYTE_CONSTANT, BYTE_LABEL, INVALID};
```

**Note:** The definition of `TokenType` is in `shared_types.hpp` due to its use
in later pipeline stages when accessing a `Line`'s vector of `Token`s.

####    $TEXT$ types

The following `TokenType`s denote subsets of strings that are in the $TEXT$ set.

#####    `INSTRUCTION`

A token string `w` is mapped to `TokenType::INSTRUCTION$ if
1.  `w` $\in TEXT$.
2.  `w` $\in INSTRUCTIONS$.
    *   This is checked by seeing whether `w` is a key of the
        `std::unordered_set<<std::string, std::vector<AssemblyInstruction>> instructions` hashmap in the `Assembler` class.

#####   `GREGISTER`

A token string `w` is mapped to `TokenType::GREGISTER` if
1.  `w` $\in TEXT$.
2.  `w` $\in GREGISTERS$.


#####   `OUT_PRINT_TYPE`

A token string `w` is mapped to `TokenType::OUT_PRINT_TYPE` if
1.  `w` $\in TEXT$.
2.  `w` is `A` or `I` (denoting ASCII print type or INTEGER print type).

#####   `TEXT`

A token string `w` is mapped to `TokenType::TEXT` if
1.  `w` $\in TEXT$.
2.  `w` cannot be mapped to any of the above $TEXT$ types.

####    `SREGISTER`

A token string `w` is mapped to `TokenType::SREGISTER` if
1.  `w.length() >= 2`.
2.  `w[0] = '%'`.
3.  `w[1:end]` $\in SREGISTERS$.

####    `NUMBER`

A token string `w` is mapped to `TokenType::NUMBER` if
1.  `w` $\in NUMBER$.

####    `IMMEDIATE`

A token string `w` is mapped to `TokenType::IMMEDIATE` if
1.  `w.length() >= 2`.
2.  `w[0] = '#'`.
3.  `w[1:end]` $\in NUMBER$.

####    `DIRECTIVE`

A token string `w` is mapped to `TokenType::DIRECTIVE` if
1.  `w.length() >= 2`.
2.  `w[0] = '.'`.
3.  `w[1:end]` $\in TEXT$.

####    `LABEL`

A token string `w` is mapped to `TokenType::LABEL` if
1.  `w.length() >= 2`.
2.  `w[0] = ':'`.
3.  `w[1:end]` $\in TEXT$.

####    `BYTE_CONSTANT`

A token string `w` is mapped to `TokenType::BYTE_CONSTANT` if
1.  There exists a string `u` $\in TEXT$ and a string `v` $\in BYTE\_SELECT$
    such that `w = uv`.

####    `BYTE_LABEL`

A token string `w` is mapped to `TokenType::BYTE_LABEL` if
1.  There exist strings `u` and `v` such that:
    1.  `u` can be mapped to `TokenType::LABEL`,
    2.  `v` $\in BYTE\_SELECT$, and
    3.  `w = uv`.

####    `INVALID`

A token string `w` is mapped to `TokenType::INVALID` if
1.  `w` cannot be mapped to any `TokenType` that is not `TokenType::INVALID`.

####    Quantity Types

Each `AssemblyInstruction` has a `std::vector<TokenType> expected_param_types`
vector that specifies the number and argument types of argument `Token`s that
the instruction expects.

E.g., for the assembly instruction `ADD R1, R2, R3`, the expected parameter
types are all of type `TokenType::GREGISTER`, and so the `expected_param_types`
vector is `{TokenType::GREGISTER, TokenType::GREGISTER, TokenType::GREGISTER}`.

However, consider the instruction `ADD R1, #5`. This instruction expects a
general purpose register and then an immediate. Hence, its 
`expected_param_types` vector is `{TokenType::GREGISTER, TokenType::IMMEDIATE}`.

Given that immediates can be expressed in multiple ways, specifically as
immediates (e.g., `#0x5`), preprocessor constants (e.g., `PI`), and 
byte-selections of labels (e.g, `:main[1]`) and preprocessor constants
(e.g., `PI[2]`), we have a bit of a problem as potentially valid invocations
such as `ADD R1, PI` will be marked as a mismatch since the token string `PI` is
mapped to `TokenType::TEXT` and *not* to `TokenType::IMMEDIATE`.

To avoid this, the Parser has a set of "quantity types" which are defined as the
set `{TokenType::IMMEDIATE, TokenType::TEXT, TokenType::BYTE_CONSTANT, TokenType::BYTE_LABEL}`.

If an argument's `TokenType` is a quantity type and its corresponding expected
argument type is also a quantity type, then the Parser will mark these as
matching even if their `TokenType`s are different. This essentially states that
the `AssemblyInstruction` expects a "quantity" for that particular argument, and
the source of the quantity is irrelevant. This allows for a single definition of
the instruction `ADD GR, immediate` with the expected parameter types vector
`{TokenType::GREGISTER, TokenType::IMMEDIATE}` without needing to explicitly
specify that, for instance, `TokenType::TEXT` is also allowed.

This means that, for `ADD GR, immediate`, the Parser will accept the following
invocations:
*   `ADD R1, #4`        (immediate)
*   `ADD R1, :main[1]`  (label byte (high byte selected))
*   `ADD R1, PI`        (constant (understood by the Parser as TEXT))
*   `ADD R1, PI[5]`     (constant byte (6th byte selected))

But the following invocations are disallowed:
*   `ADD R1, R2`        (second argument cannot be a general register)
*   `ADD R1, :main`     (second argument may not be a label)
*   `ADD R1, LDR`       (second argument may not be another instruction)

This also ensures that an assembly instruction that expects a label will always
get exactly a label, as `TokenType::LABEL` is not a quantity type. For instance,
`BEQ :label` has the expected parameter types vector `{TokenType::LABEL}`, so
the invocation `BEQ :main` is allowed, but `BEQ #5`, `BEQ :main[0]`, and
`BEQ ADDRESS_CONSTANT` are not allowed.

As branch instructions only use labels, the programmer cannot hardcode branch
addresses. This restriction both promotes good programming practice (since all
branch addresses are labels, the code is easier to read) and also avoids undue
bugs as the ByteFrost assembler supports assembly instruction derivation, i.e.,
assembly instructions are *derived into* ISA instructions, and some assembly
instructions may be derived into multiple ISA instructions, so the programmer
should not frequently rely on counting instructions to calculate jump sizes.

### `Token`

A `Token` is a struct containing a `TokenType` enum and a token string:

```cpp
struct Token {
    TokenType type;
    std::string token_string;
}
```

**Note:** The definition of `Token` is in `shared_types.hpp` due to its use
in later pipeline stages.

### `LineType`

The Parser recognizes the following `LineType`s:

```cpp
enum LineType = {INSTRUCTION, DIRECTIVE, LABEL_DEFINITION, EMPTY, INVALID};
```

####    `INSTRUCTION`

An `std::vector<Token> tokens` vector is mapped to `LineType::INSTRUCTION` if
1.  `tokens.size() >= 1`.
2.  `tokens[0].type == TokenType::INSTRUCTION`.
3.  For exactly one instruction that has the name `tokens[0].token_string`,
    the tokens in the rest of the `tokens` vector (i.e.,
    `tokens[1]...tokens[tokens.size() - 1]`) match in type with the expected
    `Token` arguments of this instruction (and the number of arguments must
    match as well).
    *   The Parser can be lenient here; see the above section on 
        "Quantity Types".

####    `DIRECTIVE`

An `std::vector<Token> tokens` vector is mapped to `LineType::DIRECTIVE` if
1. `tokens.size() >= 1`.
2. `tokens[0].type == TokenType::DIRECTIVE`.
3. There exists a preprocessor directive with the name 
    `tokens[0].token_string.substr(1)` (i.e., the directive name doesn't include
    the `.` at the start of the directive token string).
4. Each `Token`'s `TokenType` in `tokens[1]...tokens[tokens.size() - 1]` matches
    with the corresponding `TokenType` in the directive's expected `TokenType`
    list, and the number of tokens (i.e., `tokenss.size() - 1`) must match the
    number of expected token arguments of the directive.

####    `LABEL_DEFINITION`

An `std::vector<Token> tokens` vector is mapped to `LineType::LABEL_DEFINITION` 
if
1. `tokens.size() == 1`.
2. `tokens[0].type == TokenType::LABEL`.

####    `EMPTY`

An `std::vector<Token> tokens` vector is mapped to `LineType::EMPTY` if
1. `tokens.size() == 0`.

####    `INVALID`

An `std::vector<Token> tokens` vector is mapped to `LineType::INVALID` if
1. `tokens` cannot be mapped to any `LineType` that is not `LineType::INVALID`.

### `Line`

A `Line` is a struct that contains all the information needed by the ByteFrost
Assembler about a line in the input `.asm` file to generate an assembled file.

A `Line` object is defined in the following way:

```cpp
struct Line {
    unsigned int number;
    LineType type;
    std::string original_string;
    std::vector<Token> tokens;
    uint16_t line_address;
}
```

There are a few `LineType`s that require additional information, yielding the
following inheriting types:

####    `InstructionLine`

The `InstructionLine` struct inherits from `Line` and is defined as follows,
containing a pointer to the `AssemblyInstruction` the `Line` contains.

```cpp
struct InstructionLine : Line {
    AssemblyInstruction * instruction;
}
```

####    `DirectiveLine`

The `DirectiveLine` struct inherits from `Line` and is defined as follows,
containing a pointer to the `Directive` the `Line` contains.

```cpp
struct DirectiveLine : Line {
    Directive * directive;
}
```

##  Running the Parser

##  Expanding the Parser

### How to Add New Token Types (`TokenType`s)

### How to Add New Line Types (`LineType`s)