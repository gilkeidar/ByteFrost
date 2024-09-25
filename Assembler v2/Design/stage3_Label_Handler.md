#   Stage 3 - Label Handler

This is the fourth stage of the ByteFrost Assembler pipeline.

The **Branch Handler** takes as input the `std::vector<Line *>` vector generated
by the `Parser` (which was modified by the `Preprocessor`). It then does two
things:
1.  Handle every `Line` that is a label declaration statement by adding the
    `(label name, Label)` key-value pair to its 
    `std::unordered_map<string, Label> labels` hashmap. (Done in the first pass
    through the `lines` vector)
2.  Replace every `TokenType::BYTE_LABEL` and `TokenType::LABEL` token in
    `InstructionLine`s with a `TokenType::IMMEDIATE` token whose value is the
    label's *PC address value* (i.e., a normal address logically shifted right
    by 1).
    *   **Note:** When this is done, the relevant `AssemblyInstruction`s will
        receive a `TokenType::IMMEDIATE` token instead of
        `TokenType::BYTE_LABEL` or `TokenType::LABEL`. However, 
        `TokenType::LABEL`s are not considered quantity types - won't this fail
        the `AssemblyInstruction.expected_param_types` check? No, because this
        check is only done by the `Parser`, which has already finished running.
        `AssemblyInstruction.generateCode()` does NOT check that the incoming 
        tokens' `TokenType`s match the expected `TokenType`s as if we got to
        stage 4 (Output File Generation), the `Parser` is assumed to have run
        with no errors (and so the incoming tokens' token types are correct).
        The `generateCode()` method will extract the immediate value from the
        changed `Token` (that used to be a `LABEL` or `BYTE_LABEL`) correctly.

##  Definitions

### The `LabelHandler` Class

The `LabelHandler` is defined as the following class:

```cpp
class LabelHandler {
public:
    LabelHandler();
    void run(std::vector<Line *> & lines, CommandLineArguments & args);
private:
    //  Hashmap of string -> Label
    unordered_map<string, Label> labels;
}
```

### `Label`s

Each label is encoded as a `Label` struct in the following way:

```cpp
struct Label {
    string name;
    uint16_t address;   //  Note - this is the normal address, not the PC value!

    //  Returns the PC address value (address logically shifted right by 1)
    uint16_t getPCAddress();
}
```

##  Running the `LabelHandler`

The `LabelHandler.run()` method implements the following:

```cpp
//  First pass - iterate through lines vector
for (Line * line : lines) {
    //  If current line has type LABEL_DEFINITION, add the label to the labels
    //  hashmap
    if (line->type == LineType::LABEL_DEFINITION) {
        //  Create new Label
        Label newLabel({
            getLabelName(line->tokens[0]),
            line->address
        });

        //  If label name already exists in labels hashmap, throw an error
        if (this->labels.find(newLabel.name) != this->labels.end()) {
            throwErrorLine(line->number, "Duplicate label definition.");
        }

        //  Add Label to labels hashmap
        this->labels[newLabel.name] = newLabel;
    }
}

//  Define token types to replace with immediate tokens in InstructionLines
std::unordered_set<TokenType> label_types({
    TokenType::BYTE_LABEL, TokenType::LABEL
});

//  Second pass
for (Line * line : lines) {
    //  If current line is an InstructionLine, then iterate through its tokens
    //  vector and replace tokens of type BYTE_LABEL or LABEL with an IMMEDIATE,
    //  where the value comes from the label with that name's address.
    if (line->type == LineType::INSTRUCTION) {
        for (Token * token : line->tokens) {
            if (label_types.find(token->type) == label_types.end())
                //  Token doesn't have a label token type
                continue;

            int immediate_value;
            
            if (token->type == TokenType::BYTE_LABEL) {
                //  1.  Get label name and byte index from token string.
                string labelName = getLabelNameFromByteLabel(token.token_string);
                int labelIndex = getLabelIndexFromByteLabel(token.token_string);

                //  2.  Attempt to find label in labels hashmap
                if (this->labels.find(labelName) == this->labels.end()) {
                    //  Label name not found - throw error
                    throwErrorLine(line->number, "Unknown label use.");
                }
                Label & label = this->labels[labelName];

                //  3.  Ensure that the index used can fit in the label's
                //      size
                if (labelIndex < 0 || labelIndex = LABEL_SIZE_BYTES) {
                    throwErrorLine(line->number, "Out of bounds error.");
                }

                //  4.  Get address value from label and index
                //      NOTE - the value here will be from the normal address,
                //      NOT the PC address!
                immediate_value = getByteFromInt(label.address, labelIndex);
            }
            else if (token->type == TokenType::LABEL) {
                //  1.  Attempt to find label in Label Handler's
                //      string -> Label hashmap labels
                string labelName = getLabelNameFromLabel(token.token_string);
                if (this->labels.find(labelName) == this->labels.end()) {
                    //  Label name not found - throw error
                    throwErrorLine(line->number, "Unknown label use.");
                }
                Label & label = this->labels[labelName];

                //  2.  Get immediate value from label
                //      NOTE - the value here IS THE PC ADDRESS OF THE LABEL!
                immediate_value = label.getPCAddress();
            }

            //  Replace token type with TokenType::IMMEDIATE and value with
            //  immediate string
            token.type = TokenType::IMMEDIATE;
            token.value = generateImmediateString(immediate_value);
        }
    }
}
```