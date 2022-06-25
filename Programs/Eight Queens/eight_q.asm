// Setup
LDR R0, #0	// row (R0) = 0
SMR R0, R0	// board[0] = 0
MOV R1, R0	// i (R1) = row
LMR R2, R0	// col (R2) = board[row]
// start_loop
DEC R1		// i-- (R1 = R1 - 1)
BMI #27 	// if i >= 0, continue (branch to valid if i < 0)
// Horizontal check
LMR R3, R1	// R3 = board[i]
SUB R3, R3, R2	// R3 = board[i] - col
BEQ #32		// if board[i] == col, branch to not_valid
// Diagonal check 1
LMR R3, R1	// R3 = board[i]
SUB R3, R3, R2	// R3 = board[i] - col
SUB R3, R3, R0	// R3 = board[i] - col - row
ADD R3, R3, R1	// R3 = board[i] - col - row + i
BEQ #32		// if board[i] == col + (row - i), branch to not_valid
// Diagonal check 2
LMR R3, R1	// R3 = board[i]
SUB R3, R3, R2	// R3 = board[i] - col
ADD R3, R3, R0	// R3 = board[i] - col + row
SUB R3, R3, R1	// R3 = board[i] - col + row - i
BEQ #32		// if board[i] == col - (row - i), branch to not_valid
JMP #7		// Goto start_loop
// valid:
INC R0		// row+=
LDR R2, #0	// R2 = 0
SMR R2, R0	// board[row] = 0
JMP #50		// Goto loop_check
// not_valid:
LMR R2, R0	// R2 = board[row]
INC R2		// R2 = board[row] + 1
SMR R2, R0	// board[row] = board[row] + 1
LDR R3, #8	// R3 = 8
SUB R3, R2, R3	// R3 = R2 - 8
BMI #50		// if board[row] < 8, go to loop_check
DEC R0		// row--
LMR R2, R0	// R2 = board[row]
INC R2		// R2 = board[row] + 1
SMR R2, R0	// board[row] = board[row] + 1
LDR R3, #8	// R3 = 8
SUB R3, R2, R3	// R3 = R2 - 8
BMI #50		// if board[row] < 8, go to loop_check
DEC R0		// row--
LMR R2, R0	// R2 = board[row]
INC R2		// R2 = board[row] + 1
SMR R2, R0	// board[row] = board[row] + 1
// loop_check:
LDR R3, #8	// R3 = 8
SUB R3, R0, R3	// R3 = row - 8
BMI #7		// if row < 8, branch to start_loop
// Print Solution (setup)
LDR R2, #0	// R2 = 0
LDR R1, #65	// R1 = 'A'
// print_solution:
LMR R3, R2	// R3 = board[R2]
OUT R1, A	// Print rank (letter)
OUT R3, A	// Print number (row)
INC R2		// R2++
INC R1		// R1++
LDR R3, #8	// R3 = 8
SUB R3, R2, R3	// R3 = R2 - 8
BMI #57		// if R2 < 8, go to print_solution
BRK
