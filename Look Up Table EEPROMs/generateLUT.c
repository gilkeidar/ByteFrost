#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "LUT.h"

#define EXPECTED_NUM_ARGS   3

void generateLUT(FILE * lut_file, int lut_id);

int main(int argc, char ** argv) {
    printf("=== EEPROM LUT Generator ===\n");

    //  1.  Get command-line arguments.
    //  *   There should be exactly 2 command-line arguments (i.e., argc == 3).
    //  Usage: generateLUT [LUT id] [output file name]
    if (argc != EXPECTED_NUM_ARGS) {
        //  Print usage string and exit.
        printf("Usage: generateLUT [LUT id] [output file name]\n");
        return 1;
    }

    //  2.  Input validation.
    //      1.  Argument 1: LUT id (argv[1])
    //          1.  argv[1] MUST be an integer type.
    //          2.  argv[1] MUST be in the range 
    //              [0, sizeof(luts) / sizeof(LUT) - 1].
    char * endptr;
    int lut_id = strtol(argv[1], &endptr, 10);
    char * output_file_name = argv[2];

    if (strlen(endptr) != 0 
        || lut_id < 0 || lut_id >= (sizeof(luts) / sizeof(LUT))) {
        fprintf(stderr, "ERROR: Enter a valid LUT id in the range [0, %ld].\n",
            sizeof(luts) / sizeof(LUT) - 1);
        return 1;
    }

    printf("Writing LUT %d to file %s...\n", lut_id, output_file_name);

    //  3.  Attempt to open output file for writing.
    FILE * output_file = fopen(output_file_name, "wb");

    if (output_file == NULL) {
        fprintf(stderr, "ERROR: Could not open file %s for binary writing.\n",
            output_file_name);
        return 1;
    }

    //  4.  Generate LUT and write LUT to output file.
    generateLUT(output_file, lut_id);

    //  5.  Close output file.
    fclose(output_file);

    printf("Done.\n");
}

void getNextInputState(InputState * state, int lut_id) {
    //  1.  Input validation.
    //      1.  If state is NULL, assert.
    assert(state);

    //  2.  Generate max input state based on the number of inputs of lut.
    //  *   If the LUT has m inputs, then the maximum input state (all 1s) is
    //      2^m - 1.
    LUT * lut = &(luts[lut_id]);
    InputState maxState = ((1 << (lut->numInputs)) - 1);

    if (*state >= maxState) {
        *state = 0;
    }
    else {
        *state = *state + 1;
    }
}

void generateLUT(FILE * lut_file, int lut_id) {
    //  1.  Create initial input state.
    InputState currInput = 0;

    //  2.  While there are still input states whose output states need to be
    //      written to lut_file:
    do {
        //  1.  Generate the output state for the current input state.
        OutputState currOutput = luts[lut_id].generateOutputState(currInput);

        //  2.  Write output state to file
        fwrite(&currOutput, 1, 1, lut_file);

        //  3.  Get the next input state
        getNextInputState(&currInput, lut_id);
    } while (currInput != 0);
}

//  Utility functions

int InputStateBitIsHigh(InputState state, int bitOffset) {
    return (state & (1 << bitOffset)) != 0;
}

//  LUT generation functions

OutputState ARDataBusLoadEnableGenerator(InputState state) {
    //  The AR Data Bus Load Enable LUT has the following input and output
    //  configurations:
    //  8 inputs with the following bit assignments:
    //  7 - PC Load Branch                  (ACTIVE LOW)
    //  6 - loadAR                          (control signal)
    //  5 - PC Load                         (control signal)
    //  4 - loadARHorL                      (control signal)
    //  3 - OPCODE_MAA                      (ACTIVE LOW)
    //  2 - (AR) L/H instruction operand
    //  1 - ARDest[1] instruction operand
    //  0 - ARDest[0] instruction operand
    #define PC_LD_BRANCH_OFFSET     7
    #define LOAD_AR_OFFSET          6
    #define PC_LOAD_OFFSET          5
    #define LOAD_AR_H_OR_L_OFFSET   4
    #define OPCODE_MAA_OFFSET       3
    #define AR_L_H_OP_OFFSET        2

    //  8 outputs with the following bit assignments:
    //  7 - BP[H] Load Enable                   (ACTIVE LOW)  
    //  6 - BP[L] Load Enable                   (ACTIVE LOW)  
    //  5 - SP[H] Load Enable                   (ACTIVE LOW)  
    //  4 - SP[L] Load Enable                   (ACTIVE LOW)  
    //  3 - DP[H] Load Enable                   (ACTIVE LOW)  
    //  2 - DP[L] Load Enable                   (ACTIVE LOW)  
    //  1 - DHPC Load Enable                    (ACTIVE LOW)  
    //  0 - PC[L] + PC[H] = DHPC Load Enable    (ACTIVE LOW)
    //
    //  Note: At most 1 of the outputs will be 0; all else will be 1.
    #define PC_LOAD_ENABLE_OFFSET   0


    OutputState selectedOutput = 0;

    if (!InputStateBitIsHigh(state, PC_LD_BRANCH_OFFSET)) {
        //  This means that a Data Bus write to the PC is requested by a branch
        //  instruction (Branch Absolute or Branch Relative (deprecated)).
        //  Hence, PC[L] = data bus and PC[H] = DHPC.
        selectedOutput = 1 << PC_LOAD_ENABLE_OFFSET;
    }
    else if (InputStateBitIsHigh(state, LOAD_AR_OFFSET)) {
        //  This means that the current instruction is LDA, MGA, MAA, JSR, or
        //  RTS.
        if (InputStateBitIsHigh(state, PC_LOAD_OFFSET)) {
            //  A Data Bus write to the PC is requested by JSR or RTS. 
            //  The byte (H or L) is specified by the loadARHorL control signal.
            selectedOutput = 1 << PC_LOAD_ENABLE_OFFSET;
            selectedOutput <<= 
                InputStateBitIsHigh(state, LOAD_AR_H_OR_L_OFFSET);
        }
        else {
            //  The request is made by the LDA, MGA, or MAA instruction, which
            //  means that the particular AR to write to is specified by the
            //  ARDest instruction operand.
            //  If the MAA instruction is executed (i.e., by an instruction that
            //  involves writing to both bytes of an AR), then the byte MUST be
            //  specified by the loadARHorL control signal.
            //  Otherwise, the byte is specified by the (AR) L/H instruction
            //  operand.
            selectedOutput = 1 << ((state & 0x3) << 1);
            if (!InputStateBitIsHigh(state, OPCODE_MAA_OFFSET)) {
                selectedOutput <<= 
                    InputStateBitIsHigh(state, LOAD_AR_H_OR_L_OFFSET);
            }
            else {
                selectedOutput <<= InputStateBitIsHigh(state, AR_L_H_OP_OFFSET);
            }
        }
    }

    return MAX_OUTPUT_STATE ^ selectedOutput;
}