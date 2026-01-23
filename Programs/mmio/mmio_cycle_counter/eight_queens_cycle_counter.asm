 //////////////////////////////////
// Enhance to 16bit address range
// 0x0000 .. 0x1FFF  ROM 
// 0x2000 .. 0xDFFF  RAM
// 0xE000 .. 0xFFFF  Registers (MMIO)
// Set Data Pointer to  0x4000
// Set Stack Pointer to 0x7700
//////////////////////////////////

//	Time Eight Queens in cycles:
//	1.	Reset counter.
//	2.	Latch counter and copy the cycle value to 0x2000 - 0x2003 (start time)
//	3.	Run Eight Queens
//	4.	Latch counter and copy the cycle value to 0x2004 - 0x2007 (end time)
//	5.	Print start, end times, and total number of cycles
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
.define 1 MINUS				0x2d
.define	1 PERIOD			0x2e
.define 1 ASTERISK			0x2a
.define 1 _x				0x78
.define 1 EQUALS			0x3d

.define	2 device1			0xE200
//	1.	Reset counter.
LSP	%HDP, device1[1]
SMA R0, #0x00				//	Write to 0xE200 - value written ignored.

//	2.	Latch the counter and copy the cycle value to 0x2000 - 0x2003
//		(Get start time)
SMA R0, #0x01				//	Latch counter (snapshot current value)

LMA	R0, #0x02				//	Read 32-bit value into register file
LMA R1, #0x03
LMA R2, #0x04
LMA R3, #0x05

LSP %HDP, #0x20				//	*(0x2000) = start time value
SMA R0, #0x00
SMA R1, #0x01
SMA R2, #0x02
SMA R3, #0x03

//	3.	Run Eight Queens

LSP %HDP, #0x40
LSP %HSP, #0x77
LSP %DHPC, #0x00

// Setup
LDR R0, #0	// row (R0) = 0
SMR R0, R0	// board[0] = 0
SMA R0, #9  // num_solutions = 0
:for_loop_start
MOV R1, R0	// i (R1) = row
LMR R2, R0	// col (R2) = board[row]
DEC R1      // i = row - 1
:for_loop_check
BMI :valid 	// if i >= 0, continue (branch to :valid if i < 0)
// for_loop_body
// (Horizontal check)
LMR R3, R1	// R3 = board[i]
SUB R3, R3, R2	// R3 = board[i] - col
BEQ :not_valid		// if board[i] == col, branch to :not_valid
// Diagonal check 1
SUB R3, R3, R0	// R3 = board[i] - col - row
ADD R3, R3, R1	// R3 = board[i] - col - row + i
BEQ :not_valid		// if board[i] == col + (row - i), branch to :not_valid
// Diagonal check 2
LMR R3, R1	// R3 = board[i]
SUB R3, R3, R2	// R3 = board[i] - col
ADD R3, R3, R0	// R3 = board[i] - col + row
SUB R3, R3, R1	// R3 = board[i] - col + row - i
BEQ :not_valid		// if board[i] == col - (row - i), branch to :not_valid
// for_loop_update
DEC R1              // i--
JMP :for_loop_check // goto :for_loop_check
:valid
INC R0		// row++
LDR R2, #0	// R2 = 0
SMR R2, R0	// board[row] = 0
JMP :while_loop_check		// Goto :while_loop_check
:not_valid
LMR R2, R0	// R2 = board[row]
INC R2		// R2 = board[row] + 1
SMR R2, R0	// board[row] = board[row] + 1
LDR R3, #8	// R3 = 8
SUB R3, R2, R3	// R3 = R2 - 8
BMI :while_loop_check		// if board[row] < 8, go to :while_loop_check
DEC R0		// row--
LMR R2, R0	// R2 = board[row]
INC R2		// R2 = board[row] + 1
SMR R2, R0	// board[row] = board[row] + 1
LDR R3, #8	// R3 = 8
SUB R3, R2, R3	// R3 = R2 - 8
BMI :while_loop_check		// if board[row] < 8, go to :while_loop_check
DEC R0		// row--
BMI :end    // If row < 0, goto end
LMR R2, R0	// R2 = board[row]
INC R2		// R2 = board[row] + 1
SMR R2, R0	// board[row] = board[row] + 1
:while_loop_check
//OUT R0, A   // Print row
//LMR R2, R0	// R2 = board[row]
//OUT R2, A
//OUT #0x20, A
LDR R3, #8	// R3 = 8
SUB R3, R0, R3	// R3 = row - 8
BMI :for_loop_start		// if row < 8, branch to :for_loop_start
// Print Solution (setup)
LMA R3, #9  // R3 = num_solutions
INC R3      // R3 = num_solutions + 1
SMA R3, #9  // num_solutions = num_solutions + 1
OUT R3, I       // Print solution number
OUT #0x3A, A    // Print ':'
OUT #0x20, A    // Print ' '
LDR R2, #0	// R2 = 0
LDR R1, #65	// R1 = 'A'
:print_solution
LMR R3, R2	// R3 = board[R2]
OUT R1, A	// Print rank (letter)
INC R3      // Increment the number (print 1-indexed)
OUT R3, A	// Print number (row)
INC R2		// R2++
INC R1		// R1++
LDR R3, #8	// R3 = 8
SUB R3, R2, R3	// R3 = R2 - 8
BMI :print_solution		// if R2 < 8, go to print_solution
OUT #0x10, A  // Print '\n'
// move_next
DEC R0      // row--
LMR R2, R0	// R2 = board[row]
INC R2		// R2 = board[row] + 1
SMR R2, R0	// board[row] = board[row] + 1
LDR R3, #8	// R3 = 8
SUB R3, R2, R3	// R3 = R2 - 8
BMI :for_loop_start		// if board[row] < 8, go to :for_loop_start
DEC R0		// row--
LMR R2, R0	// R2 = board[row]
INC R2		// R2 = board[row] + 1
SMR R2, R0	// board[row] = board[row] + 1
LDR R3, #8	// R3 = 8
SUB R3, R2, R3	// R3 = R2 - 8
BMI :for_loop_start		// if board[row] < 8, go to :for_loop_start
DEC R0		// row--
BMI :end    // If row < 0, goto end
LMR R2, R0	// R2 = board[row]
INC R2		// R2 = board[row] + 1
SMR R2, R0	// board[row] = board[row] + 1
:end
OUT #0x2E, A // Print '.'

//	4.	Latch counter and copy the cycle value to 0x2004 - 0x2007
//		(Get end time)
LSP	%HDP, device1[1]
SMA R0, #0x01				//	Latch counter (snapshot current value)
LMA	R0, #0x02				//	Read 32-bit value into register file
LMA R1, #0x03
LMA R2, #0x04
LMA R3, #0x05
LSP %HDP, #0x20				//	*(0x2004) = start time value
SMA R0, #0x04
SMA R1, #0x05
SMA R2, #0x06
SMA R3, #0x07

//	5.	Print start, end times, and total number of cycles
//	Print "\nSTART: " + start time + '\n'
OUT NEW_LINE, A
OUT _S, A
OUT _T, A
OUT _A, A
OUT _R, A
OUT _T, A
OUT COLON, A
OUT SPACE, A

LMA R0, #0x00
LMA R1, #0x01
LMA R2, #0x02
LMA R3, #0x03
OUT R3, I
OUT R2, I
OUT R1, I
OUT R0, I

OUT NEW_LINE, A

//	Print "END: " + end time + '\n'
OUT _E, A
OUT _N, A
OUT _D, A
OUT COLON, A
OUT SPACE, A

LMA R0, #0x04
LMA R1, #0x05
LMA R2, #0x06
LMA R3, #0x07
OUT R3, I
OUT R2, I
OUT R1, I
OUT R0, I

OUT NEW_LINE, A

//	Print "E - S: " + (end time - start time)
OUT _E, A
OUT SPACE, A
OUT MINUS, A
OUT SPACE, A
OUT _S, A
OUT COLON, A
OUT SPACE, A

//	Compute 32-bit difference and store result in 0x2008 - 0x200b
LMA R0, #0x00	//	R1 = R1 - R0 (End[0] - Start[0])
LMA R1, #0x04
SUB R1, R1, R0
SMA R1, #0x08

LMA R0, #0x01
LMA R1, #0x05
SBC R1, R1, R0
SMA R1, #0x09

LMA R0, #0x02
LMA R1, #0x06
SBC R1, R1, R0
SMA R1, #0x0a

LMA R0, #0x03
LMA R1, #0x07
SBC R1, R1, R0
SMA R1, #0x0b

//	Print result
LMA R0, #0x08
LMA R1, #0x09
LMA R2, #0x0a
LMA R3, #0x0b
OUT R3, I
OUT R2, I
OUT R1, I
OUT R0, I

BRK
