#   Stage 4 - Output File Generation

This is the 4th and last stage of the ByteFrost Assembler. The Output File
Generator is given the `std::vector<Line *> lines` vector and a reference to the
`CommandLineArguments` object, and creates and writes to an output file.

The Output File Generator is defined as follows:

```cpp
class OutputFileGenerator {
public:
    OutputFileGenerator(std::vector<Line *> lines);
    void run(CommandLineArguments & clArgs);
private:
    std::vector<Line *> lines;

    void createMLGFile(std::ofstream & output_file);
    void createBINFile(std::ofstream & output_file);
};
```

When `run()` is called, the `OutputFileGenerator` does the following:

##  1. Create an output file and open it for writing.

If the `-o` flag is set, the output file name is the value of the `-o` flag.
Otherwise, the output file name is the input file name (without the extension).
If the `-b` flag is set, the output file extension is `.bin`. Otherwise, it is
`.mlg`.

Create an output file with the specified name and open it for writing.

##  2. Write to the output file.

If the `-b` flag is set, call `createBINFile()` and pass the `std::ofstream`
object for the output file to it; otherwise, call `createMLGFile()` and do the
same.

### 2.1 Write to a `.bin` file

### 2.2 Write to a `.mlg` file

##  3. Close the output file

Close the output file, and return.