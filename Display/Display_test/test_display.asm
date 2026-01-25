// Test Display interface

LDR R2, #0
:loop
OUT R2, A
INC R2
JMP :loop