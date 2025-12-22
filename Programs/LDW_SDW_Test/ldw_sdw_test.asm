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
BRK  // Check that SP=0x3258
LDA %SP, H, #0x32
LDA %SP, L, #0x58

//	3.	SP Test
//		1.	Load a value to R0.
LDR R0, #0x28

//		2.	Push R0 to the stack (now SP is at 0x3201 due to the current stack
//			implementation being empty-ascending).
PUSH R0
// Demonstrate LDA on the Address Display 
// BRK // (to visually inspect the address bus of SP = 1101, 2202, 3303, 4404, 5505, 6606)
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


//		3.	Use LDW to read the value at SP - 1 (0x3200 where R0's value was
//			just pushed).
LDW R1, %SP, #-1

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

//		4.	Use SDW to write 0x55 at 0x6605.
LDR R2, #0x55
SDW R2, %SP, #-1

//		5.	Pop the value from the stack to R0 (will pop the value at 0x6605).
POP R0

//		6.	Verify that R0's value is the value that SDW wrote at 0x3200.
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

BRK  // The PC Counter ofsets the first bit so the loaded address is shifted left before use.
// In address 6052 put a jump back to this location
// That should look like 
// 3B 00 1B AA   // PC=0158

LDA %DP, H, #0x60
LDA %DP, L, #0x52
LDR R3, #0x3B
SDW R3, %DP, #0
LDR R3, #0x00
SDW R3, %DP, #1
LDR R3, #0x1B
SDW R3, %DP, #2
LDR R3, #0xAC
SDW R3, %DP, #3


     // Now, Jump to PC=6052. The values loaded to PC are shifted by 1 60 -> 30 and 52 -> 29. 
LDA %PC, H, #0x30
LDA %PC, L, #0x29

//			printf("PC2 R0: %d R1: %d\n", R0, R1); (instructions: 12)
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

BRK