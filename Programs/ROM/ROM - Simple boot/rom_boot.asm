////////////////////////////////////////////////////////////
//	ROM Boot Code
//	Assumes:
//		1.	A BFSv1 filesystem is in use and the ByteFrost disk file used on the
//			disk has its boot sector correctly set up with first mutable code.

//	This ROM Boot Code does the following:
//	0. Run BIST
//  1.	Load the boot sector from disk into RAM (lowest possible pages)
//		*	The boot sector in disk is 2 KB (2 KB * (4 pages / 1 KB) = 8 pages)
//		*	ByteFrost memory address space:
//				First 8 KB are ROM		(pages 0 - 31)
//				Middle 48 KB are RAM	(pages 32 - 223)
//				Last 8 KB are MMIO		(pages 224 - 255)
//		->	Load pages 32 - 39 with the boot code
//	2.	Jump to the boot code.
////////////////////////////////////////////////////////////

.define	2 sdcard 			0xE000
.define	2 device1			0xE200
.define	2 device2			0xE400
.define	2 device3			0xE600
.define	2 device4			0xE800
.define	2 device5			0xEA00
.define	2 device6			0xEC00
.define	2 device7			0xEE00
.define 2 stack_address 	0xDF00  // Stack starts from DEFF downwards.
.define 2 stack_pointer 	0xDF00  // Resore SP from DF00-DF01 (different name for clarity)
  
//  0. BIST
LDA %SP, H, stack_address[1]
LDA %SP, L, stack_address[0]
CALL :bist


//	1.	Print "BFSv1 ROM BOOT\n"

OUT '\n'
OUT 'B'
OUT 'o'
OUT 'o'
OUT 't'
OUT 'R'
OUT 'O'
OUT 'M'
OUT ' '
OUT 'v'
OUT '1'
OUT '.'
OUT '1'
OUT '\n'


//	2.	Copy first 8 pages from disk to pages 32 - 39

LDA %DP, H, sdcard[1]
LDA %DP, L, sdcard[0]

//	Set SDCard command
//	Byte 0: Page
//	Byte 1: Sector High (3 lsb)
//	Byte 2: Sector Low
//	Byte 3: Go
//	SD Card Command format (16 bit command):
//	15	14	13	12	11	10-0
//	R/W	?	?	?	?	Sector ID

//	R0: i = 0 to 7 (page counter)
//	R1: SDCard command byte (0 - 3)
//	R2: SDCard command byte value

LDR R0, #0		    //	for (i = 0; i < 8; i++)
:for_loop 		
LDR R2, #32		    //	(Page = 32 + i)
ADD R2, R2, R0
SDW R2, %DP, #0     // Byte 0: Page = 32 + i
LDR R2, #0x00	    //	Set Sector High = 0x00
SDW R2, %DP, #1     // Byte 1: Sector High = 0
SDW R0, %DP, #2     // Byte 2: Sector Low  = i
SDW R2, %DP, #3     // Byte 3: Go (R2 is ignored)
INC R0
TST R0, #8		
BNE :for_loop

//	NOTE: Currently this cannot compile. Need to update the assembler
//	to allow for something like
//	:hardcodedLabelAddress = 2000
//	then
//	JMP :hardcodedLabelAddress
.define 2	RAM_START	0x2000
BRK
JMP :__RAM_START__



/////////////////////////////////////////////////////////////////
// BIST
:bist
// BIST is not using SP as a Stack Pointer, so it needs to store it and restore it at the end 
LDA %DP, H, stack_pointer[1]
LDA %DP, L, stack_pointer[0]
MAG R3, %SP, H
MAG R2, %SP, L
SDW R3, %DP, #1
SDW R2, %DP, #0


OUT '\n'   // Space from previous run on the minicom
OUT '\n'
OUT 'B'
OUT 'I'
OUT 'S'
OUT 'T'
OUT ' '
OUT 'v'
OUT '3'
OUT '.'
OUT '3'
OUT '\n'

/////////////////////////////////// Op Code 0x1B - LDA
// LDA is used in conditional /unconditional jumps, hence it is a foundational
// Op code that should be tested first

:lda_test

.define 2 address_1	0x1234
.define 2 address_2	0xFEDC

OUT 'O'
OUT 'p'
OUT #0x1B, I
OUT ':'
OUT ' '
OUT 'L'
OUT 'D'
OUT 'A'
OUT '\n'

// Test Addr 1
LDR R1, address_1[1]
LDR R0, address_1[0]

// DP
LDA %DP, H, address_1[1]
LDA %DP, L, address_1[0]
MAG R3, %DP, H
MAG R2, %DP, L

TST R3, R1
BNE :print_bad_pointer
TST R2, R0
BNE :print_bad_pointer

// BP
LDA %BP, H, address_1[1]
LDA %BP, L, address_1[0]
MAG R3, %BP, H
MAG R2, %BP, L

TST R3, R1
BNE :print_bad_pointer
TST R2, R0
BNE :print_bad_pointer

// SP
LDA %SP, H, address_1[1]
LDA %SP, L, address_1[0]
MAG R3, %SP, H
MAG R2, %SP, L

TST R3, R1
BNE :print_bad_pointer
TST R2, R0
BNE :print_bad_pointer

// Test Addr 2
LDR R1, address_2[1]
LDR R0, address_2[0]

// DP
LDA %DP, H, address_2[1]
LDA %DP, L, address_2[0]
MAG R3, %DP, H
MAG R2, %DP, L

TST R3, R1
BNE :print_bad_pointer
TST R2, R0
BNE :print_bad_pointer

// BP
LDA %BP, H, address_2[1]
LDA %BP, L, address_2[0]
MAG R3, %BP, H
MAG R2, %BP, L

TST R3, R1
BNE :print_bad_pointer
TST R2, R0
BNE :print_bad_pointer

// SP
LDA %SP, H, address_2[1]
LDA %SP, L, address_2[0]
MAG R3, %SP, H
MAG R2, %SP, L

TST R3, R1
BNE :print_bad_pointer
TST R2, R0
BNE :print_bad_pointer

JMP :maa_test

:print_bad_pointer
OUT 'E'
OUT 'x'
OUT 'p'
OUT ':'
OUT ' '
OUT R1, I
OUT R0, I
OUT '\n'
OUT 'G'
OUT 'o'
OUT 't'
OUT ':'
OUT ' '
OUT R3, I
OUT R2, I
OUT '\n'
BRK

/////////////////////////////////// Op Code 0x18-0x19 - MAA
// MAA is tested similarly to LDA
:maa_test

OUT 'O'
OUT 'p'
OUT #0x18, I
OUT '-'
OUT #0x19, I
OUT ':'
OUT ' '
OUT 'M'
OUT 'A'
OUT 'A'
OUT '\n'

.define 2 address_3	0x4567

LDR R1, address_3[1]
LDR R0, address_3[0]

// All transitions without PC: DP->BP->SP->DP->SP->SP->BP->BP->DP->DP
LDA %DP, H, address_3[1]
LDA %DP, L, address_3[0]
MAA %BP, %DP, #1   //address_3 + 1
MAA %SP, %BP, #2   //address_3 + 3
MAA %DP, %SP, #3   //address_3 + 6
MAA %SP, %DP, #4   //address_3 + 10
MAA %SP, %SP, #5   //address_3 + 15
MAA %BP, %SP, #6   //address_3 + 21
MAA %BP, %BP, #7   //address_3 + 28
MAA %DP, %BP, #8   //address_3 + 36
MAA %DP, %DP, #9   //address_3 + 45

// Now check: 
//    SP = address_3 + 15
ADD R0, #0xF
MAG R3, %SP, H
MAG R2, %SP, L

TST R3, R1
BNE :print_bad_pointer
TST R2, R0
BNE :print_bad_pointer

//    BP = address_3 + 28
ADD R0, #0xD
MAG R3, %BP, H
MAG R2, %BP, L

TST R3, R1
BNE :print_bad_pointer
TST R2, R0
BNE :print_bad_pointer

//    DP = address_3 + 45
ADD R0, #0xA
ADD R0, #0x7
MAG R3, %DP, H
MAG R2, %DP, L

TST R3, R1
BNE :print_bad_pointer
TST R2, R0
BNE :print_bad_pointer

JMP :nop_test
//////////////////////////////////// Op Code 0x00 - NOP
:nop_test

OUT 'O'
OUT 'p'
OUT #0, I
OUT ':'
OUT ' '

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

// Work with BP instead of DP because DP is the default address and
// It is hard to observe when the address is with "intent"
LDA %BP, H, counter_base[1]
LDA %BP, L, counter_base[0]
SDW R0, %BP, #0     // Reset Counter

NOP
NOP
NOP
NOP
NOP   // Time 5 NOPS. Counter = 5 * 2 + 4 = 14 (0xE)

SDW R0, %BP, #1  // Latch Counter
// LDW R0, %DP, #5  // Print Byte4
// OUT R0, I
LDW R0, %BP, #4  // Print Byte3
OUT R0, I
LDW R1, %BP, #3  // Print Byte2
OUT R1, I
LDW R2, %BP, #2  // Print Byte1
OUT R2, I
LDR R3, #0xE
TST R2, R3
BNE :FAIL

OUT ' '

SDW R0, %BP, #0     // Reset Counter

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


SDW R0, %BP, #1  // Latch Counter
// LDW R0, %DP, #5  // Print Byte4
// OUT R0, I
LDW R0, %BP, #4  // Print Byte3
OUT R0, I
LDW R1, %BP, #3  // Print Byte2
OUT R1, I
LDW R2, %BP, #2  // Print Byte1
OUT R2, I
LDR R3, #0x18
TST R2, R3
BNE :FAIL

OUT '\n'
////////////////////////////////////

//////////////////////////////////// Op Code 0x01 - BRK 
OUT 'O'
OUT 'p'
OUT #1, I
OUT ':'
OUT ' '
OUT 'B'
OUT 'R'
OUT 'K'
OUT '-'
OUT 'S'
OUT 'K'
OUT 'I'
OUT 'P'
OUT '\n'
////////////////////////////////////

//////////////////////////////////// Op Code 0x02 - ALU 
OUT 'O'
OUT 'p'
OUT #2, I
OUT ':'
OUT ' '
OUT 'A'
OUT 'L'
OUT 'U'
OUT '\n'

LDR R0, #0x27       // Don't change!
LDR R1, #0xE6       // Don't change!

////////////////////////////////////////////

// 0 -	OR

OUT 'O'
OUT 'R'
OUT ','
OR  R2, R1, R0   
LDR R3, #0xE7
TST R2, R3
BNE :FAIL

////////////////////////////////////////////

// 1 - 	AND

OUT 'A'
OUT 'N'
OUT 'D'
OUT ','
AND  R2, R1, R0   
LDR R3, #0x26
TST R2, R3
BNE :FAIL

////////////////////////////////////////////

// 2 - 	XOR

OUT 'X'
OUT 'O'
OUT 'R'
OUT ','
XOR  R2, R1, R0   
LDR R3, #0xC1
TST R2, R3
BNE :FAIL

////////////////////////////////////////////

// 2.1- NOT

OUT 'N'
OUT 'O'
OUT 'T'
OUT ','
NOT  R2, R0   
LDR R3, #0xD8
TST R2, R3
BNE :FAIL

////////////////////////////////////////////

// 3 - 	ADD
// 7 -  ADC	- C = A + B + Cin

OUT '\n'
OUT 'A'
OUT 'D'
OUT 'D'
OUT '-'
OUT 'A'
OUT 'D'
OUT 'C'
 
ADD  R2, R1, R0   // 10D = E6 + 27
BCC :FAIL         // Fail if Cary is not set
OUT '.'           // Pass 1 check 
ADC  R3, R0, R1   // Test the Carry  (0x10E0D = 27E6 + E627) 
BCC :FAIL         // Fail if Cary is not set
OUT '.'           // Pass 2 check 

LDR R0, #0x0E
TST R0, R3
BNE :FAIL
OUT '.'        // Pass 3 check 
LDR R0, #0x0D
TST R0, R2
BNE :FAIL
OUT '.'        // Pass 4 check 
LDR R0, #0x27     // Restore Settings 

////////////////////////////////////////////

// 3.1- SUB
// 7.1- SBC - C = A - B - NOT(Cin)

OUT '\n'
OUT 'S'
OUT 'U'
OUT 'B'
OUT '-'
OUT 'S'
OUT 'B'
OUT 'C'

SUB  R2, R1, R0   // BF = E6 - 27
BCC :FAIL         // Borrow is inverse of Carry. No Borrow, so Carry is set. Fail if Cary is not set
OUT '.'        // Pass 1 check 
SBC  R3, R0, R0   // Test the Borrow (0x00BF = 0x27E6 - 0x2727) 
TST R3, #0x00
BNE :FAIL
OUT '.'        // Pass 2 check 
LDR R3, #0xBF
TST R2, R3
BNE :FAIL
OUT '.'        // Pass 3 check 
// Now repeat bu with Borrow (0xFF41 = 0x2727 - 0x27E6)
SUB  R2, R0, R1   // 41 + Borrow = 27 - E6
BCS :FAIL         // Borrow is inverse of Carry. Borrow is needed so Carry is clear. Fail if Cary is not clear
OUT '.'        // Pass 4 check 
SBC  R3, R0, R0   // Test the Borrow (0xFF41 = 0x2727 - 0x27E6) 
INC R3            // Using Borrow, R3 should be 0xFF instead of 0x0    
TST R3, #0x0
BNE :FAIL
OUT '.'        // Pass 5 check 
LDR R3, #0x41
TST R2, R3
BNE :FAIL
OUT '.'        // Pass 6 check 
OUT '\n'

////////////////////////////////////////////

// 4 - 	ASL - Shift Left (msb goes to Cout, lsb gets 0)

OUT 'A'
OUT 'S'
OUT 'L'
OUT ','

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

OUT 'R'
OUT 'O'
OUT 'L'
OUT ','

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
OUT 'L'
OUT 'S'
OUT 'R'
OUT ','

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
OUT 'A'
OUT 'S'
OUT 'R'
OUT ','

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
OUT 'R'
OUT 'O'
OUT 'R'

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
OUT '\n'

//////////////////////////////////// Op Code 0x03 - LDR 
OUT 'O'
OUT 'p'
OUT #3, I
OUT ':'
OUT ' '
OUT 'L'
OUT 'D'
OUT 'R'
OUT '-'
OUT 'S'
OUT 'K'
OUT 'I'
OUT 'P'
OUT '\n'

//////////////////////////////////// Op Code 0x04 - MOV 
OUT 'O'
OUT 'p'
OUT #4, I
OUT ':'
OUT ' '
OUT 'M'
OUT 'O'
OUT 'v'

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

OUT '\n'

//////////////////////////////////// Op Code 0x05 - BRANCH ABS
OUT 'O'
OUT 'p'
OUT #5, I
OUT ':'
OUT ' '
OUT 'B'
OUT 'R'
OUT 'A'
OUT 'N'
OUT 'C'
OUT 'H'
OUT ' '
OUT 'A'
OUT 'B'
OUT 'S'
OUT '\n'

//	The conditional branches are tested twice - once where the branch
//	should be taken, and once where it should not be taken.

//	1.	JMP
OUT 'J'
OUT 'M'
OUT 'P'

JMP :JMP_PASS
JMP :FAIL

:JMP_PASS

//	2.	BMI (Branch on Minus - Negative flag must be set)
OUT ','
OUT 'B'
OUT 'M'
OUT 'I'

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
OUT ','
OUT 'B'
OUT 'C'
OUT 'S'

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
OUT ','
OUT 'B'
OUT 'E'
OUT 'Q'

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
OUT ','
OUT 'B'
OUT 'P'
OUT 'L'

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
OUT ','
OUT 'B'
OUT 'C'
OUT 'C'

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
OUT ','
OUT 'B'
OUT 'N'
OUT 'E'

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
OUT '\n'
OUT 'O'
OUT 'p'
OUT #6, I
OUT ':'
OUT ' '
OUT 'A'
OUT 'L'
OUT 'U'
OUT ' '
OUT 'I'
OUT 'M'
OUT '\n'

////////////////////////////////////////////

// 0 -	OR

OUT 'O'
OUT 'R'
OUT ','
LDR R0 #0x65
OR  R0, #8    
LDR R3, #0x6D
TST R0, R3
BNE :FAIL

LDR R0 #0x65
OR  R0, #1 //   65 = 01 OR 65   
LDR R3, #0x65
TST R0, R3
BNE :FAIL

////////////////////////////////////////////

// 1 - 	AND

OUT 'A'
OUT 'N'
OUT 'D'
OUT ','

LDR R2, #0x65
AND  R2, #0xC  	// 0x04 = 0x0C AND 0x65  
LDR R3, #0x04
TST R2, R3
BNE :FAIL

LDR R2, #0x6E
AND R2, #0xF 	// 0x0E = 0x04 AND 0x65  
LDR R3, #0x0E
TST R2, R3
BNE :FAIL

////////////////////////////////////////////

// 2 - 	XOR

OUT 'X'
OUT 'O'
OUT 'R'
OUT ','

LDR R2, #0x7B
XOR R2, #0x9  
LDR R1, #0x72  // 0x82 = 0x09 XOR 0x7B  
TST R2, R1
BNE :FAIL

LDR R0, #0x65
XOR R0, #0x4  
LDR R3, #0x61 // 0x61 = 0x04 XOR 0x65 
TST R0, R3
BNE :FAIL 


////////////////////////////////////////////

// 3 - 	ADD
// 7 -  ADC	- C = A + B + Cin

OUT 'A'
OUT 'D'
OUT 'D'
OUT '-'
OUT 'A'
OUT 'D'
OUT 'C'
OUT ','

LDR R1, #0xFE 
ADD R1, #6			// 0x104 = FE + 06
BCC :FAIL         	// Fail if Cary is not set
ADC R1, #0x05  		// 0A = 05 + 04 + 1
BCS :FAIL         	// Fail if Cary is set
LDR R0, #0x0A
TST R0, R1
BNE :FAIL

////////////////////////////////////////////

// 3.1- SUB
// 7.1- SBC - C = A - B - NOT(Cin)

OUT 'S'
OUT 'U'
OUT 'B'
OUT '-'
OUT 'S'
OUT 'B'
OUT 'C'

// Substract with Borrow 
LDR R2, #0x03 
SUB R2, #0x05  		// FE = 03 - 05 
BCS :FAIL         	// Borrow is inverse of Carry. Borrow is set, so Carry is not set. Fail if Carry is set
LDR R3, #0x03 
SBC R3, #0x02   	//  01 = 03 - 2 -Borrow  
TST R3, #0x00
BNE :FAIL
 
// Substract without Borrow 
LDR R2, #0x13 
SUB R2, #0x05  		// 0E = 13 - 05 
BCC :FAIL         	// Borrow is inverse of Carry. Borrow is not needed so Carry is set. Fail if Cary is not set
SBC R2, #0x07      	// 0x07 = 0x0E - 0x07
TST R2, #0x07
BNE :FAIL
OUT '\n'

/////////////////////////////////// Op Code 0x1A - MAG, 0x1C - MGA
//   
.define 2 mag_mga_result	0x5000


LDA %DP, H, mag_mga_result[1]
LDA %DP, L, mag_mga_result[0]
LDR R2, #0
SDW R2, %DP, #0     // Reset mag_mga_result

:mag_mga
OUT 'O'
OUT 'p'
OUT #0x1A, I
OUT '-'
OUT #0x1C, I
OUT ':'
OUT ' '
OUT 'M'
OUT 'A'
OUT 'G'
OUT '-'
OUT 'M'
OUT 'G'
OUT 'A'
OUT '\n'

LDR R0, #0xFF
:mag_mga_loop
MGA %BP, H, R0
MGA %BP, L, R0
MGA %DP, H, R0
MGA %DP, L, R0
MGA %SP, H, R0
MGA %SP, L, R0

MAG R3, %BP, H
TST R3, R0
BNE :print_bad_line
MAG R3, %BP, L
TST R3, R0
BNE :print_bad_line

MAG R3, %DP, H
TST R3, R0
BNE :print_bad_line
MAG R3, %DP, L
TST R3, R0
BNE :print_bad_line

MAG R3, %SP, H
TST R3, R0
BNE :print_bad_line
MAG R3, %SP, L
TST R3, R0
BNE :print_bad_line

JMP :cont_mag_loop

:print_bad_line
				// 1234567890123456
				// R0: DP  BP   SP
				//   0000,0000,0000 
OUT R0, I
OUT ':'
OUT ' '
OUT 'D'
OUT 'P'
OUT ' '
OUT ' '
OUT 'B'
OUT 'P'
OUT ' '
OUT ' '
OUT ' '
OUT 'S'
OUT 'P'
OUT '\n'
					// "  0000,0000,0000"
OUT ' '		
OUT ' '		
MAG R3, %DP, H
OUT R3, I
MAG R3, %DP, L
OUT R3, I
OUT ','

MAG R3, %BP, H
OUT R3, I
MAG R3, %BP, L
OUT R3, I
OUT ','

MAG R3, %SP, H
OUT R3, I
MAG R3, %SP, L
OUT R3, I

OUT '\n'
LDA %DP, H, mag_mga_result[1]
LDA %DP, L, mag_mga_result[0]
LDW R2, %DP, #0      
INC R2
SDW R2, %DP, #0

:cont_mag_loop
TST R0, #0
BEQ :after_mag_mga
DEC R0
JMP :mag_mga_loop

:after_mag_mga

LDA %DP, H, mag_mga_result[1]
LDA %DP, L, mag_mga_result[0]
LDW R2, %DP, #0      
TST R2, #0
BEQ :push_pop
OUT 'F'
OUT 'A'
OUT 'I'
OUT 'L'
OUT ' '
OUT R2, I
OUT ' '
OUT 'T'
OUT 'I'
OUT 'M'
OUT 'E'
OUT 'S'
OUT '\n'
JMP :FAIL
//////////////////////////////////// Op Code 0x0E - PUSH, 0x0F - POP
//   
//   
:push_pop
.define 1, stack_test_length 0xE0
.define 2 stack_head	0x4500
OUT 'O'
OUT 'p'
OUT #0x0E, I
OUT '-'
OUT #0x0F, I
OUT ':'
OUT ' '
OUT 'P'
OUT 'U'
OUT 'S'
OUT 'H'
OUT '-'
OUT 'P'
OUT 'O'
OUT 'P'
OUT '\n'

LDA %SP, H, stack_head[1]
LDA %SP, L, stack_head[0]

LDR R1, stack_test_length

// Push Loop 0x4500 -> 0x4410 
:push_loop
PUSH R1
DEC R1 
BNE :push_loop

LDR R3, stack_test_length
// Pop & Check loop
:pop_loop
INC R1
POP R2
TST R2, R1
BNE :local_fail
TST R1, R3
BNE :pop_loop
 
JMP :ldw_sdw
:local_fail
OUT 'E'
OUT 'x'
OUT 'p'
OUT ' '
OUT R1, I
OUT ' '
OUT 'G'
OUT 'o'
OUT 't'
OUT ' '
OUT R2, I
OUT '\n'
JMP :FAIL
///// Op Code 0x14,0x15 - LDW
//   
//  This test relies on the test PUSH 

:ldw_sdw
//Set PASS/FAIL flag
LDR R2, #0   // FAIL flag

LDA %SP, H, stack_head[1]
LDA %SP, L, stack_head[0]
LDA %DP, H, stack_head[1]
LDA %DP, L, stack_head[0]
LDA %BP, H, stack_head[1]
LDA %BP, L, stack_head[0] 

OUT 'O'
OUT 'p'
OUT #0x14, I
OUT '-'
OUT #0x15, I
OUT ':'
OUT ' '
OUT 'L'
OUT 'D'
OUT 'W'

OUT '\n'

// The loop over the three pointers must be seperated incase 
// the pointer selection does not work properly
LDR R0 stack_test_length
:ldw_loop_dp
LDW R1, %DP, #-1 // Push decrease SP before inserting data
                 // So the data start at -1 of the SP base 
TST R1, R0
BEQ :ldw_cont_dp

OUT R0, I
OUT '#'
OUT ' '
OUT 'D'
OUT 'P'
OUT '*'
OUT R1, I
OUT '\n'
LDR R2, #1
:ldw_cont_dp
MAA %DP, %DP, #-1
DEC R0
BNE :ldw_loop_dp

LDR R0 stack_test_length
:ldw_loop_bp
LDW R1, %BP, #-1 // Push decrease SP before inserting data
                 // So the data start at -1 of the SP base 
TST R1, R0
BEQ :ldw_cont_bp

OUT R0, I
OUT '#'
OUT ' '
OUT 'B'
OUT 'P'
OUT '*'
OUT R1, I
OUT '\n'
LDR R2, #1
:ldw_cont_bp

MAA %BP, %BP, #-1
DEC R0
BNE :ldw_loop_bp

LDR R0 stack_test_length
:ldw_loop_sp
LDW R1, %SP, #-1 // Push decrease SP before inserting data
                 // So the data start at -1 of the SP base 
TST R1, R0

BEQ :ldw_cont_sp
OUT R0, I
OUT '#'
OUT ' '
OUT 'S'
OUT 'P'
OUT '*'
OUT R1, I
OUT '\n'
LDR R2, #1
:ldw_cont_sp

MAA %SP, %SP, #-1
DEC R0
BNE :ldw_loop_sp
TST R2, #0
BEQ :sdw_test
JMP :FAIL
////////////////////////////////////////////
:sdw_test 
OUT 'O'
OUT 'p'
OUT #0x16, I
OUT '-'
OUT #0x17, I
OUT ':'
OUT ' '
OUT 'S'
OUT 'D'  
OUT 'W'

OUT '\n'
LDR R2, #0   // FAIL flag
.define 2 test_area 0x6666
.define 1 loop_length 0x88
LDA %SP, H, test_area[1]
LDA %SP, L, test_area[0]
LDA %DP, H, test_area[1]
LDA %DP, L, test_area[0]
LDA %BP, H, test_area[1]
LDA %BP, L, test_area[0] 

LDR R0, loop_length
:sdw_loop
SDW R0, %DP, #0 
LDW R1, %DP, #0
TST R1,R0
BEQ :sdw_cont_1
OUT 'S'
OUT 'D'
OUT 'W'
OUT ' ' 
OUT 'D'
OUT 'P'
OUT ' '
OUT R0, I
OUT ':'
OUT R1, I
OUT '\n'
LDR R2, #1 // Fail flag
:sdw_cont_1
SDW R0, %BP, #1 
LDW R1, %BP, #1
TST R1,R0
BEQ :sdw_cont_2
OUT 'S' 
OUT 'D'
OUT 'W'
OUT ' '
OUT 'B'
OUT 'P'
OUT ' '
OUT R0, I
OUT ':'
OUT R1, I
OUT '\n'
LDR R2, #1 // Fail flag
:sdw_cont_2
SDW R0, %SP, #2 
LDW R1, %SP, #2
TST R1,R0
BEQ :sdw_cont_3
OUT 'S' 
OUT 'D'
OUT 'W'
OUT ' '
OUT 'S'
OUT 'P'
OUT ' '
OUT R0, I
OUT ':'
OUT R1, I
OUT '\n'
LDR R2, #1 // Fail flag
:sdw_cont_3
DEC R0
BNE :sdw_loop
TST R2, #0
BEQ :tst_imm_test
JMP :FAIL

////////////////////////////////////////////
:tst_imm_test 
OUT 'O'
OUT 'p'
OUT #0x13, I
OUT ':'
OUT ' '
OUT 'T'
OUT 'S'  
OUT 'T'
LDR R3 #0
TST R3, #0
BNE :FAIL
LDR R3 #1
TST R3, #1
BNE :FAIL
LDR R3 #2
TST R3, #2
BNE :FAIL
LDR R3 #3
TST R3, #3
BNE :FAIL
OUT '.'
LDR R2 #4
TST R2, #4
BNE :FAIL
LDR R2 #5
TST R2, #5
BNE :FAIL
LDR R2 #6
TST R2, #6
BNE :FAIL
LDR R2 #7
TST R2, #7
BNE :FAIL
OUT '.'
LDR R1 #8
TST R1, #8
BNE :FAIL
LDR R1 #9
TST R1, #9
BNE :FAIL
LDR R1 #10
TST R1, #10
BNE :FAIL
LDR R1 #11
TST R1, #11
BNE :FAIL
OUT '.'
LDR R0 #12
TST R0, #12
BNE :FAIL
LDR R0 #13
TST R0, #13
BNE :FAIL
LDR R0 #14
TST R0, #14
BNE :FAIL
LDR R0 #15
TST R0, #15
BNE :FAIL
OUT '.'
OUT '\n'
////////////////////////////////////////////
:tst_call_jsr_rst 
OUT 'O'
OUT 'p'
OUT #0x10, I
OUT '-'
OUT #0x11, I
OUT ':'
OUT ' '

OUT 'J'
OUT 'S'
OUT 'R'
OUT ' '
OUT 'R'
OUT 'T'
OUT 'S'
OUT '\n'

.define 2 stack_head2		0xAAAA  

.define 2 recursion_level 	0x6000

LDA %SP, H, stack_head2[1]
LDA %SP, L, stack_head2[0]

LDA %DP, H, recursion_level[1]
LDA %DP, L, recursion_level[0]

// Recursion level
LDR R3, #6
SDW R3, %DP, #0

OUT 'P'
OUT 'U'
OUT 'S'
OUT 'H'
OUT ' '
CALL :recursion
OUT '\n'
OUT 'B'
OUT 'a'
OUT 'c'
OUT 'k'
OUT '!'
OUT '\n'
JMP :PASS

:recursion
LDA %DP, H, recursion_level[1]
LDA %DP, L, recursion_level[0]
LDW R3, %DP, #0

OUT R3, I
OUT ' '

PUSH R3

DEC R3
BNE :keep_going
// Stop condition met
LDR R2 , #1
POP R3
OUT '\n'
OUT 'P'
OUT 'O'
OUT 'P'
OUT ' '
OUT R3, I
OUT ' '
TST R2, R3
BNE :FAIL
RTS   // Rolling out of deepest recursion

:keep_going
SDW R3, %DP, #0
CALL :recursion

// Retrun
POP R3

OUT R3, I
OUT ' '
INC R2
TST R2, R3
BNE :FAIL
RTS  // Rolling out of intemediate recursion step

////////////////////////////////////////////

:PASS
// BIST is not using SP as a Stack Pointer, so it needs restore it at the end 
LDA %DP, H, stack_pointer[1]
LDA %DP, L, stack_pointer[0]
LDW R3, %DP, #1
LDW R2, %DP, #0
MGA %SP, H, R3
MGA %SP, L, R2

OUT '\n'
OUT 'P'
OUT 'A'
OUT 'S'
OUT 'S'
OUT ' '
CALL :Test_duration 
RTS
///////////////////////////////////////////////
:FAIL
OUT '\n'
OUT 'F'
OUT 'A'
OUT 'I'
OUT 'L'
OUT ' '
CALL :Test_duration 
BRK
///////////////////////////////////////////////
:Test_duration
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
OUT '\n'
RTS

