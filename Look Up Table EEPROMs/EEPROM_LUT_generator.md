#   EEPROM LUT Generation

**Last Updated:** June 15, 2025

This document goes over the design and implementation of the EEPROM LUT
generator program (`elg`).

##  High-Level Design

The generator (`elg`) takes the following command-line arguments:

```bash
elg [LUT Binary File Output Name (output_file_name)] [# of 1-bit LUT inputs (m)] 
    [# of 1-bit LUT outputs (n)] [# of EEPROM address bits (m_max)]
    [# of EEPROM data output bits (n_max)] 
    [ID of LUT Truth Table Function to Use (lut_id)]
```

`elg` will then attempt to create a binary file with the name `output_file_name`
that contains the LUT specified by `lut_id`.

The `elg` pseudocode is as follows:
```c
//  1.  Get command-line arguments
//  2.  Create and open output file for binary writing
//  3.  Write LUT to file - for each current input state (in order), generate
//      the corresponding output state and write the output state to the file.
//      1.  Create the initial input state (000...0 of length m)
InputState startState = getStartInputState(m);
InputState currentInput = startState;
//      2.  While there are still output states to write to file:
while (true) {
    //      1.  Generate the output state for the current input state
    OutputState currOutput = generateOutputState(currInput, n_max);

    //      2.  Write output state to file
    writeOutputStateToFile(currOutput, outputFile);

    //      3.  Get the next input state
    currInput = getNextInputState(currInput);

    //      4.  If currInput state is start state, finished!
    if (currentInput == startState) break;
}

//  4.  Close file for writing
close(outputFile);
```

Necessary then is the precise definitions of the `InputState` and `OutputState`
types.

Here is one approach, using unsigned 32-bit integers:
```c
typedef struct InputState_t {
    char m;             //  Number of input signals in bit string (LSBs) <= 32
    uint32_t state;     //  Input State bit string
} InputState;

typedef struct OutputState_t {
    char n;             //  Number of output signals in bit string (LSBs) <= 32
    char writtenWidth;  //  Number of bits of state that must be written to file 
                        //  (n_max) (# of EEPROM data output bits)
    uint32_t state;     //  Output State bit string
} OutputState;
```

This limits `elg` to only support LUTs with a maximum of `32` bits of inputs and
`32` bits of output; since the EEPROMs we use are limited to `16` bits of inputs
and `8` bits of outputs, however, this limitation is more than sufficient.

We can also modify the output state generation based on the `lut_id` desired 
with a global array of function pointers; the desired LUT generation function 
can be specified using the `lut_id` as an index.

Each LUT generation function has the following function signature:
`OutputState f(InputState inputs, int num_data_bits)`.

Hence, these can be stored in a global array defined as
```c
OutputState (*lut_generators[]) (InputState inputs, int num_data_bits) {
    //  lut generators here
}
```

### Function Pseudocode

####    `int main(int argc, char ** argv)`

1.  Get command line arguments:
    *   There should be EXACTLY `6` command-line arguments (so that `argc == 7`)
    1.  Output File Name (`char * output_file_name`).
    2.  Number of 1-bit LUT inputs (`num_LUT_inputs`) (`m`)
    3.  Number of 1-bit LUT outputs (`num_LUT_outputs`) (`n`)
    4.  Number of EEPROM address bits (`num_address_bits`) (`m_max`)
    5.  Number of EEPROM data output bits (`num_data_bits`) (`n_max`)
    6.  LUT Truth Table ID (`lut_id`)
2.  Input Validation:
    0.  If `argc != 7`:
        1.  Print usage text and return error.
    1.  If `m < 0`, `n < 0`, `m_max < 0`, or `n_max < 0`:
    *   Invalid values for `m`, `n`, `m_max`, and `n_max`.
    2.  If `m > m_max`:
    *   This means that the number of required LUT inputs (`m`) exceeds the
        number of EEPROM address bits (`m_max`), meaning this LUT cannot fit on
        an EEPROM with `m_max` address bits.
        1.  Return error.
    3.  If `n > n_max`:
    *   This means that the number of required LUT outputs (`n`) exceeds the
        number of EEPROM data output bits (`n_max`), meaning this LUT's output
        state cannot fit in the EEPROM (the output state must be able to be
        outputted in a single cycle, i.e., in parallel).
        1.  Return error.
    4.  If `lut_id` doesn't match a valid LUT ID:
        1.  Return error.
3.  Attempt to open output file for writing.
    1.  If failed, print error and return.
4.  Generate LUT (and write LUT to file).
*   `generateLUT(FILE * lut_file, int num_LUT_inputs, int num_LUT_outputs, int num_address_bits, int num_data_bits, int lut_id)`
5.  Close output file.

####    `void generateLUT(FILE * lut_file, int num_LUT_inputs, int num_LUT_outputs, int num_address_bits, int num_data_bits, int lut_id)`

1.  Create initial input state.
    1.  `InputState startState = getStartInputState(num_LUT_inputs)`.
    2.  `InputState currInput = startState`.
2.  While there are still input states whose output states need to be written to
    `lut_file`:
    1.  Generate the output state for the current input state.
        1.  `OutputState currOutput = lut_generators[lut_id](currInput, num_data_bits)`.
    2.  Write output state to file.
        1.  `writeOutputStateToFile(currOutput, lut_file)`.
    3.  Get the next input state.
        1.  `incrementInputState(&currInput)`.
    4.  If `currInput` is the start state, then all LUT generation is complete
        (overflowed).
        1.  `if (currInput == startState)   break`.

####    `InputState getStartInputState(int num_LUT_inputs)`

1.  Return `InputState state` whose fields are set to:
    1.  `state->m = num_LUT_inputs`.
    2.  `state->state = 0`.

####    `void writeOutputStateToFile(OutputState state, FILE * lut_file)`

1.  Write `state->state` to `lut_file`.

####    `void incrementInputState(InputState * state)`

1.  If `state == NULL`, return.
2.  `(state->state)++`.

### LUT Generators

####    `OutputState generateARSelect(InputState input, int num_data_bits)`

1.  Create an `OutputState` struct with
 
##  Implementation