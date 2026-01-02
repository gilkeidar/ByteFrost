//////////////////////////////////
// BISTv3:
// Add Address Registers
//////////////////////////////////
//////////////////////////////////
// BISTv2:
// Enhance to 16bit address range
// 0x0000 .. 0x1FFF  ROM 
// 0x2000 .. 0xDFFF  RAM
// 0xE000 .. 0xFFFF  Registers (MMIO)
// Set Data Pointer to  0x4000
// Set Stack Pointer to 0x7700
//////////////////////////////////

.define	1 NEW_LINE	 		0x10
.define 1 SPACE				0x20
.define 1 LEFT_PAR			0x28
.define	1 RIGHT_PAR			0x29
.define 1 COMMA				0x2c
.define 1 ASTERISK			0x2a
.define 1 DASH				0x2d
.define 1 DOT				0x2e
.define 1 COLON				0x3a
.define 1 EQUALS			0x3d

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

.define 1 _l				0x6c
.define 1 _m				0x6d
.define 1 _n				0x6e
.define 1 _o				0x6f
.define 1 _p				0x70
.define 1 _q				0x71
.define 1 _r				0x72
.define 1 _s				0x73
.define 1 _t				0x74
.define 1 _u				0x75
.define 1 _v				0x76
.define 1 _w				0x77
.define 1 _x				0x78
.define 1 _y				0x79
.define 1 _z				0x7a


OUT _B, A
OUT _I, A
OUT _S, A
OUT _T, A
OUT SPACE, A
OUT _v, A
OUT #3, A
OUT DOT, A
OUT #0, A
OUT NEW_LINE, A


//////////////////////////////////// Op Code 0x00 - NOP
OUT _O, A
OUT _p, A
OUT #0, I
OUT COLON, A
OUT SPACE, A

// Test the number of clocks on NOPs

//	The interface of the 32-bit cycle counter is as follows:
//	Address		R/W Access?		Effect
//	0xE200		W				Reset 32-bit counter (to 0)
//	0xE201		W				Latch 32-bit counter (snapshot current value)
//	0xE202		R				Read counter byte 0 (LSB)
//	0xE203		R				Read counter byte 1
//	0xE204		R				Read counter byte 2
//	0xE205		R				Read counter byte 3 (MSB)
//	(Note that the counter value is accessed as a little-endian integer)
.define 2 counter_base	0xE200


LDA %DP, H, counter_base[1]
LDA %DP, L, counter_base[0]
SDW R0, %DP, #0     // Reset Counter

NOP
NOP
NOP
NOP
NOP   // Time 5 NOPS. Counter = 5 * 2 + 4 = 14 (0xE)

SDW R0, %DP, #1  // Latch Counter
// LDW R0, %DP, #5  // Print Byte4
// OUT R0, I
LDW R0, %DP, #4  // Print Byte3
OUT R0, I
LDW R1, %DP, #3  // Print Byte2
OUT R1, I
LDW R2, %DP, #2  // Print Byte1
OUT R2, I
LDR R3, #0xE
TST R2, R3
BNE :FAIL

OUT SPACE, A

SDW R0, %DP, #0     // Reset Counter

NOP
NOP
NOP
NOP
NOP   // Time 5 NOPS
NOP
NOP
NOP
NOP
NOP   // Time 10 NOPS Counter = 10 * 2 + 4 = 24 (0x18)


SDW R0, %DP, #1  // Latch Counter
// LDW R0, %DP, #5  // Print Byte4
// OUT R0, I
LDW R0, %DP, #4  // Print Byte3
OUT R0, I
LDW R1, %DP, #3  // Print Byte2
OUT R1, I
LDW R2, %DP, #2  // Print Byte1
OUT R2, I
LDR R3, #0x18
TST R2, R3
BNE :FAIL

OUT NEW_LINE, A
////////////////////////////////////

//////////////////////////////////// Op Code 0x01 - BRK 
OUT _O, A
OUT _p, A
OUT #1, I
OUT COLON, A
OUT SPACE, A
OUT _B, A
OUT _R, A
OUT _K, A
OUT DASH, A
OUT _S, A
OUT _K, A
OUT _I, A
OUT _P, A
OUT NEW_LINE, A
////////////////////////////////////

//////////////////////////////////// Op Code 0x02 - ALU 
OUT _O, A
OUT _p, A
OUT #2, I
OUT COLON, A
OUT SPACE, A
OUT _A, A
OUT _L, A
OUT _U, A
OUT NEW_LINE, A

LDR R0, #0x27       // Don't change!
LDR R1, #0xE6       // Don't change!

////////////////////////////////////////////

// 0 -	OR

OUT _O, A
OUT _R, A
OUT COMMA, A
OR  R2, R1, R0   
LDR R3, #0xE7
TST R2, R3
BNE :FAIL

////////////////////////////////////////////

// 1 - 	AND

OUT _A, A
OUT _N, A
OUT _D, A
OUT COMMA, A
AND  R2, R1, R0   
LDR R3, #0x26
TST R2, R3
BNE :FAIL

////////////////////////////////////////////

// 2 - 	XOR

OUT _X, A
OUT _O, A
OUT _R, A
OUT COMMA, A
XOR  R2, R1, R0   
LDR R3, #0xC1
TST R2, R3
BNE :FAIL

////////////////////////////////////////////

// 2.1- NOT

OUT _N, A
OUT _O, A
OUT _T, A
OUT COMMA, A
NOT  R2, R0   
LDR R3, #0xD8
TST R2, R3
BNE :FAIL

////////////////////////////////////////////

// 3 - 	ADD
// 7 -  ADC	- C = A + B + Cin

OUT NEW_LINE, A
OUT _A, A
OUT _D, A
OUT _D, A
OUT DASH, A
OUT _A, A
OUT _D, A
OUT _C, A
 
ADD  R2, R1, R0   // 10D = E6 + 27
BCC :FAIL         // Fail if Cary is not set
OUT DOT, A        // Pass 1 check 
ADC  R3, R0, R1   // Test the Carry  (0x10E0D = 27E6 + E627) 
BCC :FAIL         // Fail if Cary is not set
OUT DOT, A        // Pass 2 check 

LDR R0, #0x0E
TST R0, R3
BNE :FAIL
OUT DOT, A        // Pass 3 check 
LDR R0, #0x0D
TST R0, R2
BNE :FAIL
OUT DOT, A        // Pass 4 check 
LDR R0, #0x27     // Restore Settings 

////////////////////////////////////////////

// 3.1- SUB
// 7.1- SBC - C = A - B - NOT(Cin)

OUT NEW_LINE, A
OUT _S, A
OUT _U, A
OUT _B, A
OUT DASH, A
OUT _S, A
OUT _B, A
OUT _C, A

SUB  R2, R1, R0   // BF = E6 - 27
BCC :FAIL         // Borrow is inverse of Carry. No Borrow, so Carry is set. Fail if Cary is not set
OUT DOT, A        // Pass 1 check 
SBC  R3, R0, R0   // Test the Borrow (0x00BF = 0x27E6 - 0x2727) 
TST R3, #0x00
BNE :FAIL
OUT DOT, A        // Pass 2 check 
LDR R3, #0xBF
TST R2, R3
BNE :FAIL
OUT DOT, A        // Pass 3 check 
// Now repeat bu with Borrow (0xFF41 = 0x2727 - 0x27E6)
SUB  R2, R0, R1   // 41 + Borrow = 27 - E6
BCS :FAIL         // Borrow is inverse of Carry. Borrow is needed so Carry is clear. Fail if Cary is not clear
OUT DOT, A        // Pass 4 check 
SBC  R3, R0, R0   // Test the Borrow (0xFF41 = 0x2727 - 0x27E6) 

TST R3, #0xF
BNE :FAIL
OUT DOT, A        // Pass 5 check 
LDR R3, #0x41
TST R2, R3
BNE :FAIL
OUT DOT, A        // Pass 6 check 
OUT NEW_LINE, A

////////////////////////////////////////////

// 4 - 	ASL - Shift Left (msb goes to Cout, lsb gets 0)

OUT _A, A
OUT _S, A
OUT _L, A
OUT COMMA, A

ASL R2, R1   // E6 -> CC + C
LDR R3, #0xCC
TST R2, R3
BNE :FAIL
ASL R2, R2   // CC -> 98 + C
LDR R3, #0x98
TST R2, R3
BNE :FAIL
ASL R2, R2   // 98 -> 30 + C
LDR R3, #0x30
TST R2, R3
BNE :FAIL
ASL R2, R2   // 30 -> 60
LDR R3, #0x60
TST R2, R3
BNE :FAIL

////////////////////////////////////////////

// 4.1- ROL - Rotate Left (msb goes to Cout, lsb gets Cin)

OUT _R, A
OUT _O, A
OUT _L, A
OUT COMMA, A

// Clear Carry 
LDR R3, #0x00
ROL R3, R3

ROL R2, R1   // E6     -> CC + C
BCC :FAIL
ROL R2, R2   // CC + C -> 99 + C
BCC :FAIL
ROL R2, R2   // 99 + C -> 33 + C
BCC :FAIL
ROL R2, R2   // 33 + C -> 67
BCS :FAIL
ROL R2, R2   // 67     -> CE
LDR R3, #0xCE
TST R2, R3
BNE :FAIL

////////////////////////////////////////////

// 5 -  LSR - Logic Shift Right (msb gets 0, lsb goes to Cout)
OUT _L, A
OUT _S, A
OUT _R, A
OUT COMMA, A

LSR R2, R1   // E6     -> 73
BCS :FAIL
LSR R2, R2   // 73     -> 39 + C
BCC :FAIL
LSR R2, R2   // 39 + C -> 1C + C
BCC :FAIL
LSR R2, R2   // 1C + C -> 0E
BCS :FAIL
LSR R2, R2   // 0E     -> 07
LDR R3, #0x07
TST R2, R3
BNE :FAIL

////////////////////////////////////////////

// 5.1- ASR - Arithmetic Shift Right (msb sign-extended, lsb goes to Cout)
OUT _A, A
OUT _S, A
OUT _R, A
OUT COMMA, A

ASR R2, R1   // E6     -> F3
BCS :FAIL
ASR R2, R2   // F3     -> F9 + C
BCC :FAIL
ASR R2, R2   // F9 + C -> FC + C
BCC :FAIL
ASR R2, R2   // FC + C -> FE
BCS :FAIL
LDR R3, #0xFE
TST R2, R3
BNE :FAIL

////////////////////////////////////////////

// 6 -  ROR - Rotate Right Cin -> Reg -> Cout
OUT _R, A
OUT _O, A
OUT _R, A

// Clear Carry 
LDR R3, #0x00
ROR R3, R3

ROR R2, R1   // E6     -> 73
BCS :FAIL
ROR R2, R2   // 73     -> 39 + C
BCC :FAIL
ROR R2, R2   // 39 + C -> 9C + C
BCC :FAIL
ROR R2, R2   // 9C + C -> CE
BCS :FAIL
ROR R2, R2   // CE     -> 67
LDR R3, #0x67
TST R2, R3
BNE :FAIL
OUT NEW_LINE, A

//////////////////////////////////// Op Code 0x03 - LDR 
OUT _O, A
OUT _p, A
OUT #3, I
OUT COLON, A
OUT SPACE, A
OUT _L, A
OUT _D, A
OUT _R, A
OUT DASH, A
OUT _S, A
OUT _K, A
OUT _I, A
OUT _P, A
OUT NEW_LINE, A

//////////////////////////////////// Op Code 0x04 - MOV 
OUT _O, A
OUT _p, A
OUT #4, I
OUT COLON, A
OUT SPACE, A
OUT _M, A
OUT _O, A
OUT _V, A

LDR R1, #0xA5
MOV R2, R1
MOV R3, R2
MOV R0, R3

LDR R1, #0x5A

TST R1, R2
BEQ :FAIL
MOV R2, R1
TST R1, R2
BNE :FAIL

TST R2, R3
BEQ :FAIL
MOV R3, R2
TST R2, R3
BNE :FAIL
 
TST R3, R0
BEQ :FAIL
MOV R0, R3
TST R3, R0
BNE :FAIL

OUT NEW_LINE, A

//////////////////////////////////// Op Code 0x05 - BRANCH ABS
OUT _O, A
OUT _p, A
OUT #5, I
OUT COLON, A
OUT SPACE, A
OUT _B, A
OUT _R, A
OUT _A, A
OUT _N, A
OUT _C, A
OUT _H, A
OUT SPACE, A
OUT _A, A
OUT _B, A
OUT _S, A
OUT NEW_LINE, A

//	The conditional branches are tested twice - once where the branch
//	should be taken, and once where it should not be taken.

//	1.	JMP
OUT _J, A
OUT _M, A
OUT _P, A

JMP :JMP_PASS
JMP :FAIL

:JMP_PASS

//	2.	BMI (Branch on Minus - Negative flag must be set)
OUT COMMA, A
OUT _B, A
OUT _M, A
OUT _I, A

//	Branch should not be taken.
LDR R0, #4
LDR R1, #5

TST R1, R0	//	R1 - R0 = 1 > 0, minus flag not set.
BMI :FAIL

//	Branch should be taken.
TST R0, R1	//	R0 - R1 = -1 < 0, minus flag is set.
BMI :BMI_PASS
JMP :FAIL

:BMI_PASS

//	3.	BCS (Branch Carry Set - Carry flag must be set)
OUT COMMA, A
OUT _B, A
OUT _C, A
OUT _S, A

//	Branch should not be taken.
LDR R0, #0x40
ASL R0, R0	//	R0 = 0x80, Cout = 0 -> Carry flag is not set.
BCS :FAIL

//	Branch should be taken.
ASL R0, R0	//	R0 = 0x00, Cout = 1 -> Carry flag is set.
BCS :BCS_PASS
JMP :FAIL

:BCS_PASS

//	4.	BEQ (Branch Equal - Zero flag must be set)
OUT COMMA, A
OUT _B, A
OUT _E, A
OUT _Q, A

//	Branch should not be taken.
LDR R0, #0x56
LDR R1, #0x55

TST R1, R0
BEQ :FAIL

//	Branch should be taken.
INC R1
TST R1, R0
BEQ :BEQ_PASS
JMP :FAIL

:BEQ_PASS

//	5.	BPL (Branch on Plus - Negative flag must not be set (really, should be Branch on Non-Negative))
OUT COMMA, A
OUT _B, A
OUT _P, A
OUT _L, A

//	Branch should not be taken.
LDR R0, #0x55
LDR R1, #0x56

TST R0, R1	//	R0 - R1 = -1 -> Negative flag is set.
BPL :FAIL

//	Branch should be taken.
TST R1, R0	//	R1 - R0 = 1 -> non-negative (negative flag not set).
BPL :BPL_PASS1
JMP :FAIL

:BPL_PASS1

//	Branch should be taken
TST R0, R0	//	R0 - R0 = 0 -> non-negative (negative flag not set).
BPL :BPL_PASS2
JMP :FAIL

:BPL_PASS2

//	6.	BCC (Branch Carry Clear - Carry flag must not be set)
OUT COMMA, A
OUT _B, A
OUT _C, A
OUT _C, A

//	Branch should not be taken.
LDR R0, #0x05
ASR R0, R0	//	R0 = 0x02, Cout = 1

BCC :FAIL

//	Branch should be taken.
ASR R0, R0	//	R0 = 0x01, Cout = 0
BCC :BCC_PASS
JMP :FAIL

:BCC_PASS

//	7.	BNE (Branch Not Equal - Zero flag must not be set)
OUT COMMA, A
OUT _B, A
OUT _N, A
OUT _E, A

//	Branch should not be taken.
LDR R0, #0x47
LDR R1, #0x47

TST R1, R0
BNE :FAIL

//	Branch should be taken.
DEC R1
TST R1, R0
BNE :BNE_PASS
JMP :FAIL
:BNE_PASS

//////////////////////////////////// Op Code 0x06 - ALU Immediate
// The immediate is only 4 bit sign extended, so the range is [-8 .. 7] 
// Only two parameter operations are applicable 
OUT NEW_LINE, A
OUT _O, A
OUT _p, A
OUT #6, I
OUT COLON, A
OUT SPACE, A
OUT _A, A
OUT _L, A
OUT _U, A
OUT SPACE, A
OUT _I, A
OUT _m, A
OUT NEW_LINE, A

////////////////////////////////////////////

// 0 -	OR

OUT _O, A
OUT _R, A
OUT COMMA, A
LDR R0 #0x65
OR  R0, #8 // 8 is extended to F8, so  FD = F8 OR 65   
LDR R3, #0xFD
TST R0, R3
BNE :FAIL

LDR R0 #0x65
OR  R0, #1 //   65 = 01 OR 65   
LDR R3, #0x65
TST R0, R3
BNE :FAIL

////////////////////////////////////////////

// 1 - 	AND

OUT _A, A
OUT _N, A
OUT _D, A
OUT COMMA, A

LDR R2, #0x65
AND  R2, #0xC  // 0x64 = 0xFC AND 0x65  
LDR R3, #0x64
TST R2, R3
BNE :FAIL

LDR R2, #0x65
AND R2, #0x4 // 0x04 = 0x04 AND 0x65  
LDR R3, #0x04
TST R2, R3
BNE :FAIL

////////////////////////////////////////////

// 2 - 	XOR

OUT _X, A
OUT _O, A
OUT _R, A
OUT COMMA, A

LDR R2, #0x7B
XOR R2, #0x9  
LDR R1, #0x82  // 0x82 = 0xF9 XOR 0x7B  
TST R2, R1
BNE :FAIL

LDR R0, #0x65
XOR R0, #0x4  
LDR R3, #0x61 // 0x61 = 0x04 XOR 0x65 
TST R0, R3
BNE :FAIL 


JMP :PASS


////////////////////////////////////////////

// 3 - 	ADD
// 7 -  ADC	- C = A + B + Cin

OUT NEW_LINE, A
OUT _A, A
OUT _D, A
OUT _D, A
OUT DASH, A
OUT _A, A
OUT _D, A
OUT _C, A
 
ADD  R2, R1, R0   // 10D = E6 + 27
BCC :FAIL         // Fail if Cary is not set
OUT DOT, A        // Pass 1 check 
ADC  R3, R0, R1   // Test the Carry  (0x10E0D = 27E6 + E627) 
BCC :FAIL         // Fail if Cary is not set
OUT DOT, A        // Pass 2 check 

LDR R0, #0x0E
TST R0, R3
BNE :FAIL
OUT DOT, A        // Pass 3 check 
LDR R0, #0x0D
TST R0, R2
BNE :FAIL
OUT DOT, A        // Pass 4 check 
LDR R0, #0x27     // Restore Settings 

////////////////////////////////////////////

// 3.1- SUB
// 7.1- SBC - C = A - B - NOT(Cin)

OUT NEW_LINE, A
OUT _S, A
OUT _U, A
OUT _B, A
OUT DASH, A
OUT _S, A
OUT _B, A
OUT _C, A

SUB  R2, R1, R0   // BF = E6 - 27
BCC :FAIL         // Borrow is inverse of Carry. No Borrow, so Carry is set. Fail if Cary is not set
OUT DOT, A        // Pass 1 check 
SBC  R3, R0, R0   // Test the Borrow (0x00BF = 0x27E6 - 0x2727) 
TST R3, #0x00
BNE :FAIL
OUT DOT, A        // Pass 2 check 
LDR R3, #0xBF
TST R2, R3
BNE :FAIL
OUT DOT, A        // Pass 3 check 
// Now repeat bu with Borrow (0xFF41 = 0x2727 - 0x27E6)
SUB  R2, R0, R1   // 41 + Borrow = 27 - E6
BCS :FAIL         // Borrow is inverse of Carry. Borrow is needed so Carry is clear. Fail if Cary is not clear
OUT DOT, A        // Pass 4 check 
SBC  R3, R0, R0   // Test the Borrow (0xFF41 = 0x2727 - 0x27E6) 

TST R3, #0xF
BNE :FAIL
OUT DOT, A        // Pass 5 check 
LDR R3, #0x41
TST R2, R3
BNE :FAIL
OUT DOT, A        // Pass 6 check 
OUT NEW_LINE, A

////////////////////////////////////////////




////////////////////////////////////////////

:PASS
OUT NEW_LINE, A
OUT _P, A
OUT _A, A
OUT _S, A
OUT _S, A
OUT SPACE, A

LDA %DP, H, counter_base[1]
LDA %DP, L, counter_base[0]

SDW R0, %DP, #1  // Latch Counter
// LDW R0, %DP, #5  // Print Byte4
// OUT R0, I
LDW R0, %DP, #4  // Print Byte3
OUT R0, I
LDW R1, %DP, #3  // Print Byte2
OUT R1, I
LDW R2, %DP, #2  // Print Byte1
OUT R2, I
BRK

:FAIL
OUT NEW_LINE, A
OUT _F, A
OUT _A, A
OUT _I, A
OUT _L, A
BRK