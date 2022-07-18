LDR R0, #0      // R0 = 0
LDR R1, #9      // R1 = 9
PUSH R0         // Push R0 onto the stack
OUT R0, A       // Print value of R0
INC R0          // R0++
SUB R2, R0, R1  // R2 = R0 - 9
BNE #3          // If R0 != 9, goto 3
OUT #0x20, A    // Print ' '
POP R3          // Pop top value from stack and store in R3
OUT R3, A       // Print R3
DEC R0          // R0--
BNE #9          // If R0 > 0, goto 9
OUT #0x10, A    // Print '\n'
JMP #01
