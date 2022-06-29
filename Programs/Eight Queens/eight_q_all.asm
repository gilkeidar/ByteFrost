// Setup
LDR R0, #0	// row (R0) = 0
SMR R0, R0	// board[0] = 0
SMA R0, #9  // num_solutions = 0
// :for_loop_start
MOV R1, R0	// i (R1) = row
LMR R2, R0	// col (R2) = board[row]
DEC R1      // i = row - 1
// :for_loop_check
BMI #0x15 	// if i >= 0, continue (branch to :valid if i < 0)
// for_loop_body
// (Horizontal check)
LMR R3, R1	// R3 = board[i]
SUB R3, R3, R2	// R3 = board[i] - col
BEQ #0x19		// if board[i] == col, branch to :not_valid
// Diagonal check 1
SUB R3, R3, R0	// R3 = board[i] - col - row
ADD R3, R3, R1	// R3 = board[i] - col - row + i
BEQ #0x19		// if board[i] == col + (row - i), branch to :not_valid
// Diagonal check 2
LMR R3, R1	// R3 = board[i]
SUB R3, R3, R2	// R3 = board[i] - col
ADD R3, R3, R0	// R3 = board[i] - col + row
SUB R3, R3, R1	// R3 = board[i] - col + row - i
BEQ #0x19		// if board[i] == col - (row - i), branch to :not_valid
// for_loop_update
DEC R1              // i--
JMP #7 // goto :for_loop_check
// :valid
INC R0		// row++
LDR R2, #0	// R2 = 0
SMR R2, R0	// board[row] = 0
JMP #0x2b		// Goto :while_loop_check
// not_valid
LMR R2, R0	// R2 = board[row]
INC R2		// R2 = board[row] + 1
SMR R2, R0	// board[row] = board[row] + 1
LDR R3, #8	// R3 = 8
SUB R3, R2, R3	// R3 = R2 - 8
BMI #0x2b		// if board[row] < 8, go to :while_loop_check
DEC R0		// row--
LMR R2, R0	// R2 = board[row]
INC R2		// R2 = board[row] + 1
SMR R2, R0	// board[row] = board[row] + 1
LDR R3, #8	// R3 = 8
SUB R3, R2, R3	// R3 = R2 - 8
BMI #0x2b		// if board[row] < 8, go to :while_loop_check
DEC R0		// row--
BMI #0x53    // If row < 0, goto end
LMR R2, R0	// R2 = board[row]
INC R2		// R2 = board[row] + 1
SMR R2, R0	// board[row] = board[row] + 1
// :while_loop_check
//OUT R0, A   // Print row
//LMR R2, R0	// R2 = board[row]
//OUT R2, A
//OUT #0x20, A
LDR R3, #8	// R3 = 8
SUB R3, R0, R3	// R3 = row - 8
BMI #4		// if row < 8, branch to :for_loop_start
// Print Solution (setup)
LMA R3, #9  // R3 = num_solutions
INC R3      // R3 = num_solutions + 1
SMA R3, #9  // num_solutions = num_solutions + 1
OUT R3, I       // Print solution number
OUT #0x3A, A    // Print ':'
OUT #0x20, A    // Print ' '
LDR R2, #0	// R2 = 0
LDR R1, #65	// R1 = 'A'
// print_solution
LMR R3, R2	// R3 = board[R2]
OUT R1, A	// Print rank (letter)
INC R3      // Increment the number (print 1-indexed)
OUT R3, A	// Print number (row)
INC R2		// R2++
INC R1		// R1++
LDR R3, #8	// R3 = 8
SUB R3, R2, R3	// R3 = R2 - 8
BMI -8		// if R2 < 8, go to print_solution
OUT #0x10, A  // Print '\n'
// move_next
DEC R0      // row--
LMR R2, R0	// R2 = board[row]
INC R2		// R2 = board[row] + 1
SMR R2, R0	// board[row] = board[row] + 1
LDR R3, #8	// R3 = 8
SUB R3, R2, R3	// R3 = R2 - 8
BMI #4		// if board[row] < 8, go to :for_loop_start
DEC R0		// row--
LMR R2, R0	// R2 = board[row]
INC R2		// R2 = board[row] + 1
SMR R2, R0	// board[row] = board[row] + 1
LDR R3, #8	// R3 = 8
SUB R3, R2, R3	// R3 = R2 - 8
BMI #4		// if board[row] < 8, go to :for_loop_start
DEC R0		// row--
BMI #0x53    // If row < 0, goto end
LMR R2, R0	// R2 = board[row]
INC R2		// R2 = board[row] + 1
SMR R2, R0	// board[row] = board[row] + 1
// end
OUT #0x2E, A // Print '.'
BRK
