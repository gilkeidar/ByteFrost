LDR R0, #0x20   // how many bytes to dump
LDR R1, #0      // Start address
LDR R2, #0x10 // New line
OUT R2, A
LDR R2, #0x3a   // ASCII ':'    
OUT R1, I       // Print address
OUT R2, A       // Print ':'
LDR R2, #0x20   // ASCII ' '    
OUT R2, A       // Print ' '
LMR R3, R1      // Load address [R1] to R3
OUT R3, I
ADD R1, #1
SUB R0, #1
BNE #16         // If there are still bytes to print
BRK
NOP 
LDR R2 #0x07   
AND R2, R2, R1   // If R1 is multiple of 8
BNE #8           // If address is not multiply, keep printing 
JMP #3           // Start a new line
