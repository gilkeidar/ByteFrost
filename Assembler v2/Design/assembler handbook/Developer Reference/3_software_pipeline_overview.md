#   Software Pipeline Overview

The ByteFrost Assembler is a complex piece of software that contains multiple
pipeline stages.

The Assembler is a CLI tool, and so its inputs are command-line arguments.
These must specify an input `.asm` file to assemble, and optionally include
additional flags to alter the operation and output of the Assembler.

<img src="black_box.png" width="300px" alt="ByteFrost Assembler Black Box">

The ByteFrost Assembler software pipeline is as follows:

<img src="assembler_pipeline.png" width="300px" alt="ByteFrost Assembler Software Pipeline">

The ByteFrost Assembler's command-line arguments are sent to the Command-Line
Argument Parser (CLAP), which parses the command-line arguments and generates a
`CommandLineArguments` object, which contains the name of the input `.asm` file
as well as which flags have been set and what their arguments are, if any.

Then, the Parser opens the input `.asm` file specified in the 
`CommandLineArguments` object for reading; it stores a copy of the file in
memory as a vector of strings, then parses each line independently, generating a
`Line` object that represents that line's semantics (e.g., whether the line is
empty, contains an assembly instruction, a preprocessor directive, or a label
definition, and additional relevant data). These `Line` objects are stored in a 
vector of `Line *`s called `lines`.

The Preprocessor then passes over the `lines` vector and handles any 
preprocessor directives as well as attempts to replace any uses of preprocessor
constants as assembly instruction arguments with their defined values.

The Label Handler also passes over the `lines` vector, keeping track of all
label declarations and replacing any uses of labels as assembly instruction
arguments with their corresponding memory addresses.

At this point, all of the assembly instruction arguments have known values, and
so the assembler begins to generate the assembled output file. With the output
file generated, the assembler finishes running.