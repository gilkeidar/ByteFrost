//	LDW and SDW Test. Runs on bare metal (not loaded from disk)

.define	1 NEW_LINE	 	0x10
.define 1 SPACE			0x20
.define	1 _A				0x41
.define	1 _B				0x42
.define	1 _C				0x43
.define	1 _D				0x44
.define	1 _E				0x45
.define	1 _F				0x46
.define	1 _G				0x47
.define	1 _H				0x48
.define	1 _I				0x49
.define	1 _J				0x4a
.define	1 _K				0x4b
.define	1 _L				0x4c
.define	1 _M				0x4d
.define	1 _N				0x4e
.define	1 _O				0x4f
.define	1 _P				0x50
.define	1 _Q				0x51
.define	1 _R				0x52
.define	1 _S				0x53
.define	1 _T				0x54
.define	1 _U				0x55
.define	1 _V				0x56
.define	1 _W				0x57
.define	1 _X				0x58
.define	1 _Y				0x59
.define	1 _Z				0x5a
.define 1 COLON				0x3a
.define 1 LEFT_PAR			0x28
.define	1 RIGHT_PAR			0x29
.define 1 COMMA				0x2c
.define 1 ASTERISK			0x2a
.define 1 _x				0x78
.define 1 EQUALS			0x3d

//	1.	Print test name ("LDW AND SDW TEST\n")
//	2.	Set high bytes of DP and SP using LSP.
//		*	PC's high byte is either 0x00 or 0x01 (likely 0x00 since this
//			program should be fairly short).
//		*	BP's high byte cannot be set using LSP.
//		*	Set DP's high byte to 0x30 and SP's high byte to 0x32 (to avoid
//			clashing).
//			*	SP's low byte starts at 0x00 (due to boot-time reset) but DP's
//				low byte contains garbage initially.
//			*	Hence, after this, without changing SP or DP, their ranges for
//				LDW and SDW are:
//				1.	SP: 0x3200 -> [0x31f0, 0x32ef]
//				2.	DP: 0x30?? -> [0x2ff0, 0x30fe] (given lowest and highest
//					values of DP's low byte, respectively).	
//	3.	SP Test
//		1.	Load a value to R0.
//		2.	Push R0 to the stack (now SP is at 0x3201 due to the current stack
//			implementation being empty-ascending).
//		3.	Use LDW to read the value at SP - 1 (0x3200 where R0's value was
//			just pushed).
//		4.	Use SDW to change the value at 0x3200 to something else.
//		5.	Pop the value from the stack to R0 (will pop the value at 0x3200).
//		6.	Verify that R0's value is the value that SDW wrote at 0x3200.
//	4.	DP Test
//		1.	Use SDW to write a value at DP and verify it is correct using LDW.
//		2.	Use SDW to write a value at DP + 127 and verify it is correct using
//			LDW.
//		3.	Use SDW to write a value at DP - 128 and verify it is correct using
//			LDW.
//	5.	PC Test
//		1.	Load byte from a particular instruction (e.g., the immediate byte)
//			using LDW, PC, #X and ensure it is the expected byte.
//
//	1.	Print test name ("LDW AND SDW TEST\n") (instructions 0 - )
OUT _L, A
OUT _D, A
OUT _W, A
OUT SPACE, A
OUT _A, A
OUT _N, A
OUT _D, A
OUT SPACE, A
OUT _S, A
OUT _D, A
OUT _W, A
OUT SPACE, A
OUT _T, A
OUT _E, A
OUT _S, A
OUT _T, A
OUT NEW_LINE, A

//	2.	Set high bytes of DP and SP using LDA.
// BRK  // Check that SP=0x3258
LDA %SP, H, #0x32
LDA %SP, L, #0x58

//	3.	SP Test
//		1.	Load a value to R0.
LDR R0, #0x28

//		2.	Push R0 to the stack (now SP is at 0x3257 due to the current stack
//			implementation being full-descending).
PUSH R0
// Demonstrate LDA on the Address Display 
// BRK // (to visually inspect the address bus of SP = 1156, 2255, 3354, 4453, 5552, 6651)
LDA %SP, H, #0x11
PUSH R0
LDA %SP, H, #0x22
PUSH R0
LDA %SP, H, #0x33
PUSH R0
LDA %SP, H, #0x44
PUSH R0
LDA %SP, H, #0x55
PUSH R0
LDA %SP, H, #0x66
PUSH R0


//		3.	Use LDW to read the value at SP (0x6651 where R0's value was
//			just pushed).
LDW R1, %SP, #0

//			printf("SPH1 R0: %d R1: %d\n", R0, R1); (instructions: 12)
OUT _S, A
OUT _P, A
OUT _H, A
OUT #1, A
OUT SPACE, A
OUT _R, A
OUT #0, A
OUT COLON, A
OUT SPACE, A
OUT R0, I
OUT SPACE, A
OUT _R, A
OUT #1, A
OUT COLON, A
OUT SPACE, A
OUT R1, I
OUT NEW_LINE, A

//		4.	Use SDW to write 0x55 at 0x6651.
LDR R2, #0x55
SDW R2, %SP, #0

//		5.	Pop the value from the stack to R0 (will pop the value at 0x6651).
POP R0

//		6.	Verify that R0's value is the value that SDW wrote at 0x6651.
//			printf("SPH2 R2: %d R0: %d\n", R2, R0);
OUT _S, A
OUT _P, A
OUT _H, A
OUT #2, A
OUT SPACE, A
OUT _R, A
OUT #2, A
OUT COLON, A
OUT SPACE, A
OUT R2, I
OUT SPACE, A
OUT _R, A
OUT #0, A
OUT COLON, A
OUT SPACE, A
OUT R0, I,
OUT NEW_LINE, A

//	4.	DP Test
//		1.	Use SDW to write a value at DP and verify it is correct using LDW.

LDR R0, #0x7e
// BRK // single step to see DP = 3030, 3033, 4040, 4044
LDA %DP, H, #0x30
LDA %DP, L, #0x30

SDW R0, %DP, #3
LDW R1, %DP, #3
LDA %DP, H, #0x40
LDA %DP, L, #0x40
SDW R0, %DP, #4
LDW R1, %DP, #4
//			printf("DP1 R0: %d R1: %d\n", R0, R1); (instructions: 12)
OUT _D, A
OUT _P, A
OUT #1, A
OUT SPACE, A
OUT _R, A
OUT #0, A
OUT COLON, A
OUT SPACE, A
OUT R0, I
OUT SPACE, A
OUT _R, A
OUT #1, A
OUT COLON, A
OUT SPACE, A
OUT R1, I
OUT NEW_LINE, A

//		2.	Use SDW to write a value at DP + 127 and verify it is correct using
//			LDW.
// BRK // Single Step to see DP = 4090, 410F, 4040, 3FF0
LDR R0, #0x25
LDA %DP, L, #0x90
SDW R0, %DP, #0x7F
LDW R1, %DP, #127
LDA %DP, L, #0x40
SDW R0, %DP, #-0x50
LDW R1, %DP, #-0x50

//			printf("DP2 R0: %d R1: %d\n", R0, R1); (instructions: 12)
OUT _D, A
OUT _P, A
OUT #2, A
OUT SPACE, A
OUT _R, A
OUT #0, A
OUT COLON, A
OUT SPACE, A
OUT R0, I
OUT SPACE, A
OUT _R, A
OUT #1, A
OUT COLON, A
OUT SPACE, A
OUT R1, I
OUT NEW_LINE, A

//		3.	Use SDW to write a value at DP - 128 and verify it is correct using
//			LDW.
// BRK // Check DP = 5050, 4FD0
LDR R0, #0x82
LDA %DP, H, #0x50
LDA %DP, L, #0x50
SDW R0, %DP, #-128
LDW R1, %DP, #-128

//			printf("DP3 R0: %d R1: %d\n", R0, R1); (instructions: 12)
OUT _D, A
OUT _P, A

OUT #3, A
OUT SPACE, A
OUT _R, A
OUT #0, A
OUT COLON, A
OUT SPACE, A
OUT R0, I
OUT SPACE, A
OUT _R, A
OUT #1, A
OUT COLON, A
OUT SPACE, A
OUT R1, I
OUT NEW_LINE, A

//	5.	PC Test
//		1.	Load byte from a particular instruction (e.g., the immediate byte)
//			using LDW, PC, #X and ensure it is the expected byte.
//			Instruction to use: LDR R0, #0x82 (high byte = #0x82)
.define	1	LDR_INSTR_OFFSET_1	-3
.define	1	LDR_INSTR_OFFSET_2	-63

LDR R0, #0x45
LDW R1, %PC, LDR_INSTR_OFFSET_1

//			printf("PC1 R0: %d R1: %d\n", R0, R1); (instructions: 12)
OUT _P, A
OUT _C, A
OUT #1, A
OUT SPACE, A
OUT _R, A
OUT #0, A
OUT COLON, A
OUT SPACE, A
OUT R0, I
OUT SPACE, A
OUT _R, A
OUT #1, A
OUT COLON, A
OUT SPACE, A
OUT R1, I
OUT NEW_LINE, A

//		2.	Test LDA perfoming JUMP

//BRK  // The PC Counter ofsets the first bit so the loaded address is shifted left before use.
// In address 6052 put a jump back to this location
// That should look like 
// 3B 00 1B AA   // PC=0158

LDA %DP, H, #0x60
LDA %DP, L, #0x52
LDR R3, #0x3B
SDW R3, %DP, #0

//  Store return address of RETURN_POINT at #0x6052.
//  Note that the ByteFrost Assembler handles labeles in the following way:
//  1.  If a LABEL is used without byte selection (i.e., as a full label, such as BEQ :RETURN_POINT), then the label immediate is replaced not with the byte address but the instruction address (already shifted right by 1).
//  2.  If a BYTE_LABEL is used (e.g., as here, LDR R3, :RETURN_POINT[1]), the raw byte is used without shifting right. However, since we want the PC to read this value, 
LDR R3, :RETURN_POINT[1] // #0x00
LSR R3, R3  // Program counter needs divided by half and the bit ned to carry over to the lower byte 
SDW R3, %DP, #1
LDR R3, #0x1B
SDW R3, %DP, #2
LDR R3, :RETURN_POINT[0] // #0xAC
ROR R3, R3
SDW R3, %DP, #3


     // Now, Jump to PC=6052. The values loaded to PC are shifted by 1 60 -> 30 and 52 -> 29. 
LDA %PC, H, #0x30
LDA %PC, L, #0x29

//	Ensure this worked by printing a different message otherwise and stopping.
//	(I.e., avoid erroneous fall-through to :RETURN_POINT)
//	printf("ERROR\n");
OUT _E, A
OUT _R, A
OUT _R, A
OUT _O, A
OUT _R, A
OUT NEW_LINE, A

BRK

//			printf("PC2 R0: %d R1: %d\n", R0, R1); (instructions: 12)
:RETURN_POINT
OUT _P, A
OUT _C, A
OUT #2, A
OUT SPACE, A
OUT _M, A 
OUT _A, A 
OUT _D, A 
OUT _E, A 
OUT SPACE, A
OUT _I, A 
OUT _T, A 
OUT NEW_LINE, A

//	Test MGA instruction.
//	1.	Set R0 = 0x65 and R1 = 0x02
LDR R0, #0x65
LDR R1, #0x02

//	2.	Set DP = {R0, R1}.
MGA %DP, H, R0
MGA %DP, L, R1

//	3.	Store R2 = 0x64 at *DP.
LDR R2, #0x64
SDW R2, %DP, #0

//	4.	R3 = *DP
LDW R3, %DP, #0

//	5.	Print MGA: R2: {R2} R3: {R3}\n
OUT _M, A
OUT _G, A
OUT _A, A
OUT COLON, A
OUT SPACE, A
OUT _R, A
OUT #2, A
OUT COLON, A
OUT SPACE, A
OUT R2, I
OUT SPACE, A
OUT _R, A
OUT #3, A
OUT COLON, A
OUT SPACE, A
OUT R3, I
OUT NEW_LINE, A

//	Test RTS instruction.
//	RTS instruction semantics:
//	0.	Assume that SP points at return address in little-endian format, i.e.
//		SP + 1: RET[H]
//		SP    : RET[L]
//	1.	SP++.
//		*	SP points at RET[H]
//	2.	DHPC = *SP; SP--.
//		*	SP points at RET[L].
//	3.	PC[L] = *SP, PC[H] = DHPC (RET[H]); SP++
//		*	SP points at RET[H] and PC has been set to RET.
//	4.	SP++
//		*	SP points at the byte before RET[H] on the stack (i.e., above it).
//	Hence, we can test RTS' implementation as follows:
//	1.	Push a return address to the stack (can be some label).
//	2.	Call RTS; should've jumped to the return address on the stack and ran
//		the code there.
//		*	If fall through, print error; if set to a random address, will have
//			undefined behavior.

LDR R3, :RTS_SUCCESS[1]
LSR R3, R3
PUSH R3
LDR R3, :RTS_SUCCESS[0]
ROR R3, R3
PUSH R3

RTS

:RTS_FAILURE

//	Print RTS FAIL
OUT _R, A
OUT _T, A
OUT _S, A
OUT SPACE, A
OUT _F, A
OUT _A, A
OUT _I, A
OUT _L, A
OUT SPACE, A

BRK

:RTS_SUCCESS

//	Print RTS SUCCESS
OUT _R, A
OUT _T, A
OUT _S, A
OUT SPACE, A
OUT _S, A
OUT _U, A
OUT _C, A
OUT _C, A
OUT _E, A
OUT _S, A
OUT _S, A
OUT NEW_LINE, A

//  Test remaining instructions:
//  1.  MAG
//  2.  MAA
//  3.  JSR

//  1.  Test MAG instruction (MAG Rd, ARSrc, L/H)
//      *   Can test with MGA / LDA
//      1.  Set DP to 0x1977 using LDA and verify using MAG.

//  DP = 0x1977
LDA %DP, H, #0x19
LDA %DP, L, #0x77

MAG R0, %DP, L
MAG R1, %DP, H

//  printf("MAG DP E: 1977 A: {R1,R0}\n");
OUT _M, A
OUT _A, A
OUT _G, A
OUT SPACE, A
OUT _D, A
OUT _P, A
OUT SPACE, A
OUT _E, A
OUT COLON, A
OUT #0x19, I
OUT #0x77, I
OUT SPACE, A
OUT _A, A
OUT COLON, A
OUT R1, I
OUT R0, I

//      2.  Set SP to 0x2022 using LDA and verify using MAG.

//  SP = 0x2022
LDA %SP, H, #0x20
LDA %SP, L, #0x22

MAG R0, %SP, L
MAG R1, %SP, H

//  printf("MAG SP E:2022 A:{R1, R0}\n");
OUT _M, A
OUT _A, A
OUT _G, A
OUT SPACE, A
OUT _S, A
OUT _P, A
OUT SPACE, A
OUT _E, A
OUT COLON, A
OUT #0x20, I
OUT #0x22, I
OUT SPACE, A
OUT _A, A
OUT COLON, A
OUT R1, I
OUT R0, I

//  2.  Test MAA instruction
//      1.  Set DP to #0x3344 and SP to DP + 127. Expectation: SP = #0x33C3

//  DP = 0x3344
LDA %DP, H, #0x33
LDA %DP, L, #0x44

//  SP = DP + 127
MAA %SP, %DP, #127

//  printf("MAA1 DP: {R1R0} E: 3344\n");
MAG R1, %DP, H
MAG R0, %DP, L

OUT _M, A
OUT _A, A
OUT _A, A
OUT #1, A
OUT SPACE, A
OUT _D, A
OUT _P, A
OUT COLON, A
OUT SPACE, A
OUT R1, I
OUT R0, I
OUT SPACE, A
OUT _E, A
OUT COLON, A
OUT #0x33, I
OUT #0x44, I

//  printf("MAA1 SP: {R1R0} E: 33C3\n");
MAG R1, %SP, H
MAG R0, %SP, L

OUT _M, A
OUT _A, A
OUT _A, A
OUT #1, A
OUT SPACE, A
OUT _S, A
OUT _P, A
OUT COLON, A
OUT SPACE, A
OUT R1, I
OUT R0, I
OUT SPACE, A
OUT _E, A
OUT COLON, A
OUT #0x33, I
OUT #0xC3, I

//      2.  Set DP to DP - 128. Expectation: DP = #0x32C4
MAA %DP, %DP, #-128

//  printf("MAA2 DP: {R1R0} E: 32C4\n");
MAG R1, %DP, H
MAG R0, %DP, L

OUT _M, A
OUT _A, A
OUT _A, A
OUT #2, A
OUT SPACE, A
OUT _D, A
OUT _P, A
OUT COLON, A
OUT SPACE, A
OUT R1, I
OUT R0, I
OUT SPACE, A
OUT _E, A
OUT COLON, A
OUT #0x32, I
OUT #0xC4, I

//  3.  Test JSR (using CALL)
BRK

CALL :function

:JSR_FAILURE
//	Print JSR FAIL
OUT _J, A
OUT _S, A
OUT _R, A
OUT SPACE, A
OUT _F, A
OUT _A, A
OUT _I, A
OUT _L, A
OUT SPACE, A

BRK

:function
//  Print CALL SUCCESS
OUT NEW_LINE, A
OUT _C, A
OUT _A, A
OUT _L, A
OUT _L, A
OUT SPACE, A
OUT _S, A
OUT _U, A
OUT _C, A
OUT _C, A
OUT _E, A
OUT _S, A
OUT _S, A

BRK