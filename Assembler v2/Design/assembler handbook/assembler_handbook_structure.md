#   ByteFrost Assembler Handbook Table of Contents

The ByteFrost Assembler Handbook is organized as two volumes: A 
`Developer Reference` volume with software structure information, as well as a
`Programmer Manual` volume with reference information useful for a ByteFrost
Assembly programmer.

##  Developer Reference

1.  Overview
2.  Definitions
    1.  General Definitions
    2.  Parsing Set Definitions
        1.  Character (Symbol) Sets
        2.  String Sets
3.  Software Pipeline Overview
4.  Stage 0 - Command-Line Argument Parser (CLAP)
    1.  Overview
    2.  Definitions
    3.  Command-Line Argument Parsing
    4.  Expanding the CLAP
        1.  How to Add New Token Types (`CLTokenType`s)
        2.  How to Add New Flags (`CLFlag`s)
5.  Stage 1 - Parser
    1.  Overview
    2.  Definitions
    3.  Running the Parser
    4.  Expanding the Parser
        1.  How to Add New Token Types (`TokenType`s)
        2.  How to Add New Line Types (`LineType`s)
6.  Stage 2 - Preprocessor
    1.  Overview
    2.  Definitions
    3.  Running the Preprocessor
    4.  Expanding the Preprocessor
        1.  How to Add New Preprocessor Directives (`Directive`s)
7.  Stage 3 - Label Handler
    1.  Overview
    2.  Definitions
    3.  Running the Label Handler
8.  Stage 4 - Code Generation
    1.  Overview
    2.  Definitions
    3.  Running the Output File Generator
    4.  Expanding the Output File Generator
        1.  How to Add New ISA Instructions (`ISAInstruction`s)
        2.  How to Add New Assembly Instructions (`AssemblyInstruction`s)
        3.  How to Add New Output File Types

##  Programmer Manual

1.  Overview
2.  Command-Line Usage
3.  Preprocessor
    1.  Directives
    2.  Constants
        1.  The `define` directive
        2.  Constants Array Syntax
4.  Labels
    1.  Label Definitions
    2.  Label Array Syntax
5.  Assembly Instructions
    1.  Assembly Instruction List

