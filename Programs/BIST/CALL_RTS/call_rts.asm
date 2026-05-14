// Test CALL RTS

OUT '\n'   // Space from previous run on the minicom
OUT '\n'
OUT 'C'
OUT 'A'
OUT 'L'
OUT 'L'
OUT ' '
OUT 'R'
OUT 'T'
OUT 'S'
OUT '\n'

.define 2 kbd_addr			0xE207

.define 2 stack_head		0xAAAA  

.define 2 recursion_level 	0x6000

LDA %SP, H, stack_head[1]
LDA %SP, L, stack_head[0]

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

BRK  // End of Program

:recursion
LDA %DP, H, recursion_level[1]
LDA %DP, L, recursion_level[0]
LDW R3, %DP, #0

OUT R3, I
OUT ' '

PUSH R3

DEC R3
BNE :keep_going
POP R3
OUT '\n'
OUT 'P'
OUT 'O'
OUT 'P'
OUT ' '
OUT R3, I
OUT ' '

// // Debug Print the stack
// LDA %BP, H, stack_head[1]
// LDA %BP, L, stack_head[0]
// LDA %DP, H, stack_head[1]
// LDA %DP, L, stack_head[0]
// LDR R0 #0x10
// :print_stack_bp
// LDW R2, %BP, #0
// OUT R2, I
// MAA %BP, %BP, #-1
// DEC R0
// BNE :print_stack_bp 
// OUT '\n'
// LDR R0 #0x10
// :print_stack_dp
// LDW R2, %DP, #0
// OUT R2, I
// MAA %DP, %DP, #-1
// DEC R0
// BNE :print_stack_dp 
// OUT '\n'

RTS

:keep_going
SDW R3, %DP, #0
CALL :recursion

// Retrun
POP R3

OUT R3, I
OUT ' '
RTS

