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
    //  9 inputs with the following bit assignments:
    //  8 - Clock signal (to avoid needing to NOR load triggers)
    //      *   This is an optimization that saves 5 - 6 NOR gates by
    //          effectively including them as part of the LUT.
    //  7 - PC Load Branch                  (ACTIVE LOW)
    //  6 - loadAR                          (control signal)
    //  5 - PC Load                         (control signal)
    //  4 - loadARHorL                      (control signal)
    //  3 - OPCODE_MAA                      (ACTIVE LOW)
    //  2 - (AR) L/H instruction operand
    //  1 - ARDest[1] instruction operand
    //  0 - ARDest[0] instruction operand
    #define CLK_OFFSET              8
    #define PC_LD_BRANCH_OFFSET     7
    #define LOAD_AR_OFFSET          6
    #define PC_LOAD_OFFSET          5
    #define LOAD_AR_H_OR_L_OFFSET   4
    #define OPCODE_MAA_OFFSET       3
    #define AR_L_H_OP_OFFSET        2

    //  8 outputs with the following bit assignments:
    //  7 - BP[H] Load Trigger                  (ACTIVE HIGH)  
    //  6 - BP[L] Load Trigger                  (ACTIVE HIGH)  
    //  5 - SP[H] Load Trigger                  (ACTIVE HIGH)
    //      *   Eventually, SP[H] will also use two 4-bit counters and will have
    //          a parallel load enable active low input so this will need to be
    //          replaced with an active low Load Enable instead of a load
    //          trigger
    //  4 - SP[L] Load Enable                   (ACTIVE LOW)  
    //  3 - DP[H] Load Trigger                  (ACTIVE HIGH)  
    //  2 - DP[L] Load Trigger                  (ACTIVE HIGH)  
    //  1 - DHPC Load Trigger                   (ACTIVE HIGH)  
    //  0 - PC[L] + PC[H] = DHPC Load Enable    (ACTIVE LOW)
    #define PC_LOAD_ENABLE_OFFSET   0

    //  For every input state with address 0x0XX:
    //      (This means that the clock signal is low, or in the second half of
    //       the cycle)
    //      1.  For each active high load trigger bit x:
    //          1.  If x should be active, it should be set to 1. Otherwise, 0.
    //      2.  For each active low load enable bit x:
    //          1.  If x should be active, it should be set to 0. Otherwise, 1.
    //  For every input state with address 0x1XX:
    //      (This means that the clock signal is high, or in the first half of
    //      the cycle. All bits should be inactive).
    //      1.  For each active high load trigger bit x:
    //          1.  x should be 0.
    //      2.  For each active low load enable bit x:
    //          1.  x should be 1.

    if (InputStateBitIsHigh(state, CLK_OFFSET)) {
        //  This means that the main clock signal is high (i.e., in the first
        //  half of the clock cycle).
        //  All bits should be inactive.
        //  TODO: Change this to include bit 5 (SP[H]) to be active low when the
        //  SP[H] register is replaced with two 4-bit counters that have load
        //  enable inputs.
        //  7 - Load Trigger (active high) - 0
        //  6 - Load Trigger (active high) - 0
        //  5 - Load Trigger (active high) - 0
        //  4 - Load Enable  (active low)  - 1
        //  3 - Load Trigger (active high) - 0
        //  2 - Load Trigger (active high) - 0
        //  1 - Load Trigger (active high) - 0
        //  0 - Load Enable  (active low)  - 1
        //  = 0x11
        return 0x11;
    }

    //  Otherwise - main clock signal is low (i.e., in the second half of the
    //  clock signal).

    if (!InputStateBitIsHigh(state, PC_LD_BRANCH_OFFSET)) {
        //  This means that a Data Bus write to the PC is requested by a branch
        //  instruction (Branch Absolute or Branch Relative (deprecated)).
        //  Hence, PC[L] = data bus and PC[H] = DHPC.
        //  
        //  Set the P[L] + PC[H] = DHPC Load Enable (active low) to 0.
        //  All other outputs should have inactive values.
        //  TODO: Change this to include bit 5 (SP[H]) to be active low when the
        //  SP[H] register is replaced with two 4-bit counters that have load
        //  enable inputs.
        //  7 - Load Trigger (active high) - 0
        //  6 - Load Trigger (active high) - 0
        //  5 - Load Trigger (active high) - 0
        //  4 - Load Enable  (active low)  - 1
        //  3 - Load Trigger (active high) - 0
        //  2 - Load Trigger (active high) - 0
        //  1 - Load Trigger (active high) - 0
        //  0 - Load Enable  (active low)  - 0  (ACTIVE)
        //  = 0x10
        return 0x10;
    }
    else if (InputStateBitIsHigh(state, LOAD_AR_OFFSET)) {
        //  This means that the current instruction is LDA, MGA, MAA, JSR, or
        //  RTS.
        if (InputStateBitIsHigh(state, PC_LOAD_OFFSET)) {
            //  A Data Bus write to the P is requested by JSR or RTS.
            //  The byte (H or L) is specified by the loadARHorL control signal.
            if (InputStateBitIsHigh(state, LOAD_AR_H_OR_L_OFFSET)) {
                //  The high byte is requested --> DHPC load trigger should be
                //  active.
                //  7 - Load Trigger (active high) - 0
                //  6 - Load Trigger (active high) - 0
                //  5 - Load Trigger (active high) - 0
                //  4 - Load Enable  (active low)  - 1
                //  3 - Load Trigger (active high) - 0
                //  2 - Load Trigger (active high) - 0
                //  1 - Load Trigger (active high) - 1  (ACTIVE)
                //  0 - Load Enable  (active low)  - 1
                return 0x13;
            }
            else {
                //  The low byte is requested --> PC[L] + PC[H] = DHPC load
                //  enable should be active.
                //  7 - Load Trigger (active high) - 0
                //  6 - Load Trigger (active high) - 0
                //  5 - Load Trigger (active high) - 0
                //  4 - Load Enable  (active low)  - 1
                //  3 - Load Trigger (active high) - 0
                //  2 - Load Trigger (active high) - 0
                //  1 - Load Trigger (active high) - 0
                //  0 - Load Enable  (active low)  - 0  (ACTIVE)
                return 0x10;
            }
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
            //  In other words:
            //  1.  The AR to write to is specified by the ARDest instruction
            //      operand.
            //  1.  If the MAA instruction is executed (OPCODE_MAA_OFFSET is 0
            //      since it is active low):
            //      1.  The byte is specified by the loadARHorL control signal.
            //  2.  Otherwise:
            //      1.  The byte is specified by the (AR) L/H instruction
            //          operand.

            //  Default state
            OutputState result = 0x11;
            
            //  Determine the right bit position that represents the AR byte to
            //  write to.

            //  Get to the bit that represents the low byte of ARDest
            OutputState active_bit = 1 << ((state & 0x3) << 1);

            //  Choose whether to stay at the low byte or move to the high byte
            if (!InputStateBitIsHigh(state, OPCODE_MAA_OFFSET)) {
                active_bit <<= 
                    InputStateBitIsHigh(state, LOAD_AR_H_OR_L_OFFSET);
            }
            else {
                active_bit <<= InputStateBitIsHigh(state, AR_L_H_OP_OFFSET);
            }

            //  XOR the bit position with the result to set it to active.
            return result ^ active_bit;
        }
    }

    //  Return default
    return 0x11;

    // //  The initial value of selected output should be such that each bit is the
    // //  negation of the default non-active value of that bit, or in other words,
    // //  it should be the active value of that bit.
    // //  For active low outputs, that is 0.
    // //  For active high outputs, that is 1.
    // //  TODO: Change this to include bit 5 (SP[H]) to be active low when the
    // //  SP[H] register is replaced with two 4-bit counters that have load
    // //  enable inputs.
    // //  7 - Load Trigger (active high) - 0
    // //  6 - Load Trigger (active high) - 0
    // //  5 - Load Trigger (active high) - 0
    // //  4 - Load Enable  (active low)  - 1
    // //  3 - Load Trigger (active high) - 0
    // //  2 - Load Trigger (active high) - 0
    // //  1 - Load Trigger (active high) - 0
    // //  0 - Load Enable  (active low)  - 1
    // OutputState selectedOutput = 0xee;

    // if (!InputStateBitIsHigh(state, PC_LD_BRANCH_OFFSET)) {
    //     //  This means that a Data Bus write to the PC is requested by a branch
    //     //  instruction (Branch Absolute or Branch Relative (deprecated)).
    //     //  Hence, PC[L] = data bus and PC[H] = DHPC.
    //     selectedOutput = 1 << PC_LOAD_ENABLE_OFFSET;
    // }
    // else if (InputStateBitIsHigh(state, LOAD_AR_OFFSET)) {
    //     //  This means that the current instruction is LDA, MGA, MAA, JSR, or
    //     //  RTS.
    //     if (InputStateBitIsHigh(state, PC_LOAD_OFFSET)) {
    //         //  A Data Bus write to the PC is requested by JSR or RTS. 
    //         //  The byte (H or L) is specified by the loadARHorL control signal.
    //         selectedOutput = 1 << PC_LOAD_ENABLE_OFFSET;
    //         selectedOutput <<= 
    //             InputStateBitIsHigh(state, LOAD_AR_H_OR_L_OFFSET);
    //     }
    //     else {
    //         //  The request is made by the LDA, MGA, or MAA instruction, which
    //         //  means that the particular AR to write to is specified by the
    //         //  ARDest instruction operand.
    //         //  If the MAA instruction is executed (i.e., by an instruction that
    //         //  involves writing to both bytes of an AR), then the byte MUST be
    //         //  specified by the loadARHorL control signal.
    //         //  Otherwise, the byte is specified by the (AR) L/H instruction
    //         //  operand.
    //         selectedOutput = 1 << ((state & 0x3) << 1);
    //         if (!InputStateBitIsHigh(state, OPCODE_MAA_OFFSET)) {
    //             selectedOutput <<= 
    //                 InputStateBitIsHigh(state, LOAD_AR_H_OR_L_OFFSET);
    //         }
    //         else {
    //             selectedOutput <<= InputStateBitIsHigh(state, AR_L_H_OP_OFFSET);
    //         }
    //     }
    // }

    // return MAX_OUTPUT_STATE ^ selectedOutput;
}

OutputState ARSelectGenerator(InputState state) {
    //  The ARSelect LUT has the following input and output configurations:
    //  8 inputs with the following bit assignments:
    //  7 - Bus Grant
    //  6 - FetchCycle
    //  5 - PC Out                          (control signal)
    //  4 - SP Out                          (control signal)
    //  3 - TmpARWrite                      (control signal)
    //  2 - OPCODE_MAG_LDW_SDW_MAA
    //  1 - ARSrc[1] instruction operand
    //  0 - ARSrc[0] instruction operand
    #define BUS_GRANT_OFFSET                    7
    #define FETCH_CYCLE_OFFSET                  6
    #define PC_OUT_OFFSET                       5
    #define SP_OUT_OFFSET                       4
    #define TMP_AR_WRITE_OFFSET                 3
    #define OPCODE_MAG_LDW_SDW_MAA_OFFSET       2

    //  5 outputs with the following bit assignments:
    //  4 - TmpAR Output Enable             (ACTIVE LOW)
    //  3 - BP Output Enable                (ACTIVE LOW)
    //  2 - SP Output Enable                (ACTIVE LOW)
    //  1 - DP Output Enable                (ACTIVE LOW)
    //  0 - PC Output Enable                (ACTIVE LOW)
    //
    //  Note: at most, 1 of these outputs will be 0; all else will be 1.
    #define TMP_AR_OUTPUT_ENABLE_OFFSET 4
    #define BP_OUTPUT_ENABLE_OFFSET     3
    #define SP_OUTPUT_ENABLE_OFFSET     2
    #define DP_OUTPUT_ENABLE_OFFSET     1
    #define PC_OUTPUT_ENABLE_OFFSET     0

    if (InputStateBitIsHigh(state, BUS_GRANT_OFFSET)) {
        //  This means that the Bus Grant signal is active.
        return MAX_OUTPUT_STATE;
    }
    
    //  If the Bus Grant signal is not active, then exactly one AR will be
    //  selected to write to the Address Bus.

    OutputState selectedOutput = 0;
    if (InputStateBitIsHigh(state, FETCH_CYCLE_OFFSET) 
        || InputStateBitIsHigh(state, PC_OUT_OFFSET)) {
        //  This means that the FetchCycle signal or PC Out is active.
        selectedOutput = 1 << PC_OUTPUT_ENABLE_OFFSET;
    }
    else if (InputStateBitIsHigh(state, SP_OUT_OFFSET)) {
        //  This means that the SP Out control signal is active.
        selectedOutput = 1 << SP_OUTPUT_ENABLE_OFFSET;
    }
    else if (InputStateBitIsHigh(state, TMP_AR_WRITE_OFFSET)) {
        //  This means that the TmpARWrite control signal is active.
        selectedOutput = 1 << TMP_AR_OUTPUT_ENABLE_OFFSET;
    }
    else if (InputStateBitIsHigh(state, OPCODE_MAG_LDW_SDW_MAA_OFFSET)) {
        //  This means that the instruction has the opcode of MAG, LDW, SDW, or
        //  MAA instructions (i.e., that it has an ARSrc instruction operand).
        selectedOutput = 1 << (state & 0x3);
    }
    else {
        //  Default behavior: Write DP to the Address Bus.
        selectedOutput = 1 << DP_OUTPUT_ENABLE_OFFSET;
    }

    return MAX_OUTPUT_STATE ^ selectedOutput;
}
