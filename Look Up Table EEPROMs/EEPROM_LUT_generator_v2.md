#   EEPROM LUT Generation v2

**Last Updated:** June 15, 2025

This document goes over the design and implementation of the EEPROM LUT
generator program (`generateLUT`).

##  High-Level Design

The generator (`generateLUT`) takes the following command-line arguments:

```bash
generateLUT [ID of LUT (lut_id)] 
            [LUT Binary File Output Name (output_file_name)]
```

`generateLUT` will then attempt to create a binary file with the name
`output_file_name` that contains the LUT specified by `lut_id`.

The following types are then defined:
```c
//  State of input bits of a LUT. 
//  The order of the input bits depends on (and must be explicitly specified by)
//  the particular LUT being generated.
//  However, the input bits must be the LSBs of the InputState in order to be
//  correct.
/// 
//  Note that the use of a uint32_t implies that the number of input signals to
//  the LUT may not exceed 32. Given that our EEPROMs have a maximum of about 16
//  address bits, this is a reasonable assumption (an EEPROM with a 32 bit input
//  would map to at least 4 Gbits or 512 MB)
typedef uint32_t InputState;

//  Output state of a LUT.
//  The order of the output bits depends on (and must be explicitly specified
//  by) the particular LUT being generated.
//  However, the output bits must be the LSBs of the OutputState in order to be
//  correct.
//  Note that the use of a uint8_t implies that the number of output signals
//  from the LUT may not exceed 8. Given that our EEPROMs have an 8-bit data
//  output, this is a reasonable assumption.
typedef uint8_t OutputState;

typedef struct LUT_t {
    //  Number of 1-bit inputs of this LUT. Must be specified by the LSBs of the
    //  InputState int.
    int numInputs;

    //  Number of 1-bit outputs of this LUT.
    int numOutputs;

    //  Number of bytes of parallel data output (in EEPROM; typically 1 byte).
    //  We'll always assume that the parallel data output is 1 byte.
    // int numDataBytes;

    //  LUT Generation function
    OutputState (*generateOutputState) (InputState input);
} LUT;

//  Global luts array
LUT luts[] = {
    //  LUT initializations here
};
```

### Function Pseudocode

####    `int main(int argc, char ** argv)`

1.  Get command-line arguments:
*   There should be exactly `2` command-line arguments (i.e., `argc == 3`)
    1.  LUT ID (`int lut_id`)
    2.  Output File Name (`char * output_file_name`)
2.  Input Validation:
    1.  If `lut_id < 0 || lut_id >= sizeof(luts) / sizeof(LUT)`:
    *   Then `lut_id` is not a valid index of the `luts` array.
        1.  Print error and return.
3.  Attempt to open output file `FILE * lut_file` for writing.
    1.  If failed, print error and return.
4.  Generate LUT (and write LUT to file).
    1.  `generateLUT(lut_file, &(luts[lut_id]))`.
5.  Close output file.

####    `void generateLUT(FILE * lut_file, LUT * lut)`

1.  Create initial input state.
    1.  `InputState currInput = 0`.
2.  While there are still input states whose output states need to be written to
    `lut_file`:
    1.  Generate the output state for the current input state.
        1.  `OutputState currOutput = lut->generateOutputState(currInput)`.
    2.  Write output state to file.
        1.  `writeOutputStateToFile(currOutput, lut_fil)`.
    3.  Get the next input state.
        1.  `getNextInputState(&currInput, lut)`.
    4.  If `currInput` is the start state (wrapped around), then LUT generation
        is complete.
        1.  `if currInput == startState: break`.
    
####    `void writeOutputStateToFile(OutputState output, FILE * lut_file)`

1.  If `lut_file` is `NULL`:
    1.  Assert.
2.  Write `output` to `lut_file`.

####    `void getNextInputState(InputState * state, LUT * lut)`

1.  If `state` or `lut` is `NULL`:
    1.  Assert.
2.  Generate max input state based on the number of inputs of `lut`.
    1.  `InputState maxState = (lut->numInputs == sizeof(InputState) * 8) ? (MAX_INT) : (1 << (lut->numInputs)) - 1;`
3.  If `*state >= maxState || (*state) + 1 >= maxState`:
    1.  `*state = 0`.
4.  Otherwise, `*state = *state + 1`.

### LUTs

####    **AR Data Bus Load Enable** LUT

**numInputs:** `8`

**numOutputs:** `8` (no decoder used)

**Input Configuration:**

| `7` | `6` | `5` | `4` | `3` | `2` | `1` | `0` |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `PC Load Branch` (active low) | *loadAR* | *PC Load* | *loadARHorL* | `Opcode_MAA` (active low) | `(AR) L/H instruction operand` | `ARDest[1]` | `ARDest[0]` |

**Output Configuration:**

| `7` | `6` | `5` | `4` | `3` | `2` | `1` | `0` |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `BP[H] Load Enable` (active low) | `BP[L] Load Enable` (active low) | `SP[H] Load Enable` (active low) | `SP[L] Load Enable` (active low) | `DP[H] Load Enable` (active low) | `DP[L] Load Enable` (active low) | `DHPC Load Enable` (active low) | `PC[L] + PC[H] = DHPC Load Enable` (active low) 

####    `OutputState ARDataBusLoadEnableGenerator(InputState state)`

1.  `uint8_t selection = 0`.
2.  If `(state & (1 << PC_LD_BRANCH_OFFSET)) == 0`:
*   A Data Bus write to the `PC` is requested by a branch instruction (Branch
    Absolute or Branch Relative (deprecated))
    1.  `selection = 1 << PC_LO_LOAD_ENABLE_OFFSET`.
3.  Else if `(state & (1 << LOAD_AR_OFFSET))`:
*   This means that the current instruction being executed is `LDA`, `MGA`,
    `MAA`, `JSR`, or `RTS`.
    1.  If `(state & (1 << PC_LOAD_OFFSET))`
    *   A Data Bus write to the `PC` is requested, but to which byte?
    *   The request is made either by `JSR` or `RTS`; the byte is specified by
        the *loadARHorL* control signal.
        1.  `selection = 1 << ((state & (1 << LOAD_AR_H_OR_L_OFFSET)) != 0)`.
    2.  Else:
    *   The request is made by the `LDA`, `MGA`, or `MAA` instruction, meaning
        that the particular AR to write to is specified by the `ARDest`
        instruction operand. Now we must identify whether the high or low byte
        is specified.
    *   If it's made by `MAA` (i.e., by an instruction that involves writing to
        both bytes of an AR), then the byte MUST be specified by the
        *loadARHorL* control signal.
    *   Otherwise, the byte is specified by the `(AR) L/H` instruction operand.
        1.  `selection = 1`.
        2.  `selection = selection << ((state & 0x3) << 1)`;
        3.  If `(state & (1 << OPCODE_MAA_OFFSET)) == 0`:
            1.  `selection = selection << ((state & LOAD_AR_H_OR_L_OFFSET) != 0)`
        4.  Else:
            1.  `selection = selection << ((state & (AR_L_H_OP_OFFSET)) != 0)`
4. Return `MAX_BYTE ^ selection`.

####    **ARSelect** LUT

**numInputs:** `8`

**numOutputs:** `5` (no decoder used)

**Input Configuration:**

| `7` | `6` | `5` | `4` | `3` | `2` | `1` | `0` |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `Bus Grant` | `FetchCycle` | *PC Out* | *SP Out* | *TmpARWrite* | `Opcode_MAG_LDW_SDW_MAA` (active low) | `ARSrc[1]` | `ARSrc[0]` |

**Output Configuration:**

| `4` | `3` | `2` | `1` | `0` |
| --- | --- | --- | --- | --- |
| `TmpAR Output Enable` (active low) | `BP Output Enable` (active low) | `SP Output Enable` (active low) | `DP Output Enable` (active low) | `PC Output Enable` (active low) |  

#####   `OutputState ARSelectGenerator(InputState state)`

1.  If `state & (1 << BUS_GRANT_OFFSET)`:
*   This means that the Bus Grant signal is active.
    1.  Return `MAX_BYTE` (`255`).
2.  `selectedAR = 0`.
3.  Else if `(state & (1 << FETCH_CYCLE_OFFSET)) || (state & (1 <<PC_OUT_OFFSET))`:
*   This means that the FetchCycle signal or *PC Out* is active.
    1.  `selectedAR = 1 << PC_OUTPUT_ENABLE_OFFSET (0)`.
4.  Else if `state & (1 << SP_OUT_OFFSET)`:
*   This means that the *SP Out* control signal is active.
    1.  `selectedAR = 1 << SP_OUTPUT_ENABLE_OFFSET (2)`.
5.  Else if `state & (1 << TMP_AR_WRITE_OFFSET)`:
*   This means that the *TmpARWrite* control signal is active.
    1.  `selectedAR = 1 << TMP_AR_OUTPUT_ENABLE_OFFSET (4)`.
6.  Else if `(state & (1 << OPCODE_MAG_LDW_SDW_MAA_ACTIVE_LOW_OFFSET)) == 0`:
*   This means that the instruction has the opcode of `MAG`, `LDW`, `SDW`, or
    `MAA` instructions (i.e., that it has an `ARSrc` instruction operand).
    1.  `selectedAR = 1 << (state & 0x3)`.
7.  Else:
*   Default behavior: Write `DP` to the bus.
    1.  `selectedAR = 1 << DP_OUTPUT_ENABLE_OFFSET (1)`.
8. Return `MAX_BUTE ^ selectedAR`.

