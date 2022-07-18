LDR R0, #10             // n = 10
JSR #0x05               // R0 = fibonacci(n)
OUT R0, i               // print R0
BRK                     // BRK
OUT R0, i               // Print n (R0)
LDR R1, #2              // R1 = 2
SUB R1, R0, R1          // R1 = n - 2
BPL #0x0b               // if (n < 2) {
LDR R0, #1              //      R0 = 1;
RTS                     //      return R0; }
DEC R0                  // R0--
MOV R2, R0              // R2 = R0
PUSH R2                 // Push R2
JSR #0x05               // R0 = fibonacci(n - 1)
POP R2                  // Pop R2
MOV R3, R0              // R3 = R0
DEC R2                  // R2--
MOV R0, R2              // R0 = R2
PUSH R2                 // Push R2
PUSH R3                 // Push R3
JSR #0x05               // R0 = fibonacci(n - 2)
POP R3                  // Pop R3
POP R2                  // Pop R2
ADD R0, R0, R3          // R0 = fibonacci(n - 1) + fibonacci(n - 2)
RTS                     // return R0
