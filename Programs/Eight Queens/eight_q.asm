//////////////////////////////////
// Enhance to 16bit address range
// 0x0000 .. 0x1FFF  ROM 
// 0x2000 .. 0xDFFF  RAM
// 0xE000 .. 0xFFFF  Registers (MMIO)
// Set Data Pointer to  0x4000
// Set Stack Pointer to 0x7700
//////////////////////////////////

LSP %DP, #0x40
LSP %SP, #0x77

// Setup
LDR R0, #0	// row (R0) = 0
SMR R0, R0	// board[0] = 0
// :for_loop_start
MOV R1, R0	// i (R1) = row
LMR R2, R0	// col (R2) = board[row]
DEC R1      // i = row - 1
// :for_loop_check
BMI #0x14 	// if i >= 0, continue (branch to :valid if i < 0)
// for_loop_body
// (Horizontal check)
LMR R3, R1	// R3 = board[i]
SUB R3, R3, R2	// R3 = board[i] - col
BEQ #0x18		// if board[i] == col, branch to :not_valid
// Diagonal check 1
SUB R3, R3, R0	// R3 = board[i] - col - row
ADD R3, R3, R1	// R3 = board[i] - col - row + i
BEQ #0x18		// if board[i] == col + (row - i), branch to :not_valid
// Diagonal check 2
LMR R3, R1	// R3 = board[i]
ADD R3, R3, R0	// R3 = board[i] + row
SUB R3, R3, R2	// R3 = board[i] +row - col
SUB R3, R3, R1	// R3 = board[i] - col + row - i
BEQ #0x18		// if board[i] == col - (row - i), branch to :not_valid
// for_loop_update
DEC R1              // i--
JMP #0x06 // goto :for_loop_check
// :valid
INC R0		// row++
LDR R2, #0	// R2 = 0
SMR R2, R0	// board[row] = 0
JMP #0x29		// Goto :while_loop_check
// not_valid
LMR R2, R0	// R2 = board[row]
INC R2		// R2 = board[row] + 1
SMR R2, R0	// board[row] = board[row] + 1
LDR R3, #8	// R3 = 8
SUB R3, R2, R3	// R3 = R2 - 8
BMI #0x29		// if board[row] < 8, go to :while_loop_check
DEC R0		// row--
LMR R2, R0	// R2 = board[row]
INC R2		// R2 = board[row] + 1
SMR R2, R0	// board[row] = board[row] + 1
LDR R3, #8	// R3 = 8
SUB R3, R2, R3	// R3 = R2 - 8
BMI #0x29		// if board[row] < 8, go to :while_loop_check
DEC R0		// row--
LMR R2, R0	// R2 = board[row]
INC R2		// R2 = board[row] + 1
SMR R2, R0	// board[row] = board[row] + 1
// :while_loop_check
OUT R0, A   // Print row
LMR R2, R0	// R2 = board[row]
OUT R2, A
OUT #0x20, A
LDR R3, #8	// R3 = 8
SUB R3, R0, R3	// R3 = row - 8
BMI #0x03		// if row < 8, branch to :for_loop_start
// Print Solution (setup)
LDR R2, #0	// R2 = 0
LDR R1, #65	// R1 = 'A'
// print_solution
LMR R3, R2	// R3 = board[R2]
OUT R1, A	// Print rank (letter)
OUT R3, A	// Print number (row)
INC R2		// R2++
INC R1		// R1++
LDR R3, #8	// R3 = 8
SUB R3, R2, R3	// R3 = R2 - 8
BMI -7		// if R2 < 8, go to print_solution
BRK
