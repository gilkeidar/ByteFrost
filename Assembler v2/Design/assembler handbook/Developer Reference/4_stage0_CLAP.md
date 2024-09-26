#   Stage 0 - Command-Line Argument Parser (CLAP)

##  Overview

This is the first stage of the ByteFrost Assembler pipeline.

Given the command-line arguments as specified by `int argc`, `char ** argv`
received by running the ByteFrost Assembler on a command-line shell, the
**Command-Line Argument Parser (CLAP)** generates a `CommandLineArguments`
object that contains the specified input file and any additional command-line
argument flags / values. The CLAP will throw an error or warning for any
command-line argument misuse. The generated `CommandLineArguments` is allocated
by the CLAP on the heap and a pointer to it is returned to the `Assembler`.

##  Definitions

### `CLTokenType`

The CLAP recognizes the following Command-Line Argument Token (`CLToken`) types,
which are the values of the `CLTokenType` enum:

```cpp
enum CLTokenType = {FLAG, NUMBER, FILE_NAME, TEXT, INVALID};
```

####    `FLAG`

A command-line argument string `w` is mapped to `CLTokenType::FLAG` if
1.  `w.length() >= 2`.
2.  `w[0] = '-'`.
3.  `w[1:end]` $\in TEXT$.

####    `NUMBER`

A command-line argument string `w` is mapped to `CLTokenType::NUMBER` if 
1. `w` $\in NUMBER$.

####    `FILE_NAME`

A command-line argument string `w` is mapped to `CLTokenType::FILE_NAME` if
1. `w` $\in FILE$.

####    `TEXT`

A command-line argument string `w` is mapped to `CLTokenType::TEXT` if
1. `w` $\in TEXT$.

####    `INVALID`

A command-line argument string `w` is mapped to `CLTokenType::INVALID` if
1. `w` cannot be mapped to any `CLTokenType` that is not `CLTokenType::INVALID`.

### `CLToken`

A `CLToken` is a struct containing a `CLTokenType` enum and a token string.

Every token string received by the CLAP as part of the command-line arguments
array `char ** argv` is mapped to a `CLTokenType` and stored in a `CLToken`.

```cpp
struct CLToken {
    CLTokenType type;
    std::string token_string;
}
```

### `CFlag`

The `CFlag` struct is defined as follows:

```cpp
struct CFLag {
    std::string flag_name;
    std::vector<CLTokenType> expected_pattern;

    //  Command-line flag's argument values filled by the CLAP if each 
    //  argument's CLTokenType matches the corresponding CLTokenType in the
    //  expected_pattern vector
    std::vector<CLToken> values;
    bool is_set = false;
}
```

The following are flags (`CFlag`s) that are recognized by the CLAP:

####    `-b`    Binary Flag
*   Use: When set, this flag tells the ByteFrost Assembler to produce a `.bin`
    file.
*   Flag name: `b`.
*   Number of arguments: 0.
*   Expected pattern: `{}`.

####    `-o`    Output File Name Flag
*   Use: When set, this flag tells the ByteFrost Assembler to produce an output
    file with the name specified by the following argument (`CLToken`).
*   Flag name: `o`.
*   Number of arguments: 1.
*   Expected pattern: `{FILE}`.

##  Command-Line Argument Parsing

The CLAP is given the command-line arguments in the form of the tuple
`(int argc, char ** argv)`, passed to it by the `Assembler` constructor. This is
similiar to the `std::vector<std::string> token_strings` intermediate data input
in the Parser (Stage 1) parsing of `.asm` file line strings.

The entirety of the `CLAP`'s algorithm is performed in the `CLAP::run()` method.
A similar design is done for all pipeline stages.

The CLAP will first convert the command-line argument strings into a list of
`CLToken`s (i.e., each token string is mapped to a `CLToken` to generate a
`std::vector<CLToken> tokens` vector).

Each command-line argument string `w` is mapped to a `CLTokenType`, and then a
correponding `CLToken` object is created with the contents of `w` and the
specified `CLTokenType`. This is done by the `stringToCLToken()` function. The
generated `CLToken` object is then added to the `std::vector<CLToken> tokens`
vector.

If a command-line argument string `w` is mapped to `CLTokenType::INVALID`, then
the CLAP throws an error.

Once the `std::vector<CLToken>` list is generated, the CLAP will iterate over
this list and treat each token as a flag, a flag argument, or as the required
input argument which is the input file name.

The first argument that is of type `CLTokenType::FILE`, ends with the string
`".asim"`, and is not na argument of a previous flag token, will be understood
by the CLAP to be the required input file name argument.

When a flag argument is encountered, the CLAP will check whether the flag is
recognized by the CLAP by checking whether the argument string exists as a key
of the flag name (`std::string`) to `CLFlag` hashmap 
`std::unordered_map<std::string, CLFlag> flags` in the `CommandLineArguments`
object allocated by the CLAP.

If the flag is not recognized (key doesn't exist), the CLAP will throw an error.
If the flag does exist, then the corresponding `CLFlag` object's `is_set` field
will be set to `true` and the object will contain a
`vector<CLTokenType expected_pattern>` vector which details the number and types
of the following expected `CLToken` arguments. If there aren't enough `CLToken`s
after this one or if their types won't match, the CLAP will throw an error. The
CLAP will also set the `CLFlag` argument value fields to the values in the
following `CLToken`s (e.g., if we have `-size 3`, then the `size` `CLFlag`
object will store copies of these `CLToken`s in its 
`std::vector<CLToken> values` vector).

If an argument is encountered that cannot be understood as the input file name,
a flag, or a flag argument, the CLAP will throw an error.

Once the CLAP finishes iterating through the `CLToken` vector, it will check to
ensure that it received an input file name (as it is a required argument); if
it did not, it will throw an error.

If no errors have been thrown, the CLAP will return a pointer to the allocated
and filled `CommandLineArguments` object to the `Assembler`.

##  Expanding the CLAP

Here are some remarks on how one may expand the CLAP's capabilities.

### How to Add New Token Types (`CLTokenType`s)

To add a `CLTokenType`, do the following:
1.  Add the new `CLTokenType` value to the enum declaration in `clap.hpp`.
2.  Update the `stringToCLToken()` function to correctly map token strings that
    should be understood as the new `CLTokenType` to that `CLTokenType`.
3.  Update the `CLTokenTypeToString()` method with the new `CLTokenType`.
    (this is for debug completeness).

### How to Add New Flags (`CLFlag`s)

To add a new `CLFlag`, do the following:
1.  Add a `#define` constant in `constants.hpp` with the flag name string.
    (e.g., `#define SIZE_FLAG_NAME  "size"`)
2.  Update the `CommandLineArguments` constructor, adding the new `CLFlag` to
    the `flags` hashmap.