#   Stage 2 - Preprocessor

##  Overview

This is the third stage of the ByteFrost Assembler pipeline.

The **Preprocessor** takes as input the `std::vector<Line *>` vector generated
by the `Parser`. It then does two things:
1.  Handle every `Line` that contains a `Directive` statement (i.e., every
    `DirectiveLine`) by updating the preprocessor's or the `Assembler`'s state.
2.  Replace every `Token` that could be a preprocessor constant (i.e., every
    `Token` with type `TokenType::TEXT` or `TokenType::BYTE_CONSTANT`, as these
    are assumed by the `Parser` to be preprocessor constants defined in 
    `.define` `Directive` `Line`s) with a `Token` of type `TokenType::IMMEDIATE`
    and whose value is the immediate value of the used constant.

##  Definitions

### `DirectiveType`

Each directive that the Preprocessor recognizes has its own `DirectiveType` enum
value:

```cpp
enum class DirectiveType {define, ...};
```

### `Directive`

Each `Preprocessor` directive is encoded as a `Directive` struct as follows:

```cpp
struct Directive {
    std::string name;
    DirectiveType type;
    std::vector<TokenType> expected_param_types;
}
```

The `preprocessor.hpp` file contains a `const` array of all Preprocessor
`Directive`s.

The `Assembler` has a `std::string` -> `Directive *` hashmap
`std::unordered_map<std::string, Directive *> directives` that is passed to the
`Parser` (like the `instructions` hashmap) in order for it to generate
`DirectiveLine`s, defined in the section *Stage 1 - Parser*.

### `PreprocessorConstant`

A `PreprocessorConstant` is a struct that represents a constant defined using
the `.define` preprocessor directive. All instances of `Token`s representing 
a preprocessor constant known by the Preprocessor are replaced with an immediate
`Token` with the constant's value.

The `PreprocessorConstant` is defined in the following way:

TODO:   FIX THIS SECTION AND TYPEDEF THE `long long` as 
`PreprocessorConstantSize` or something!!

```cpp
struct PreprocessorConstant {
    std::string name;
    size_t size;
    long long value;
}
```

### The `Preprocessor` Class

The `Preprocessor` class is defined in the following way:

```cpp
class Preprocessor {
public:
    Preprocessor();
    //  run() method that executes the Preprocessor pipeline stage
    void run(std::vector<Line *> & lines, CommandLineArguments & args);
private:
    //  Hashmap of string -> PreprocessorConstant
}
```

##  Running the Preprocessor

##  Expanding the Preprocessor

### How to Add New Preprocessor Directives (`Directive`s)