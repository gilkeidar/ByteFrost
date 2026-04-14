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

.define 2 kbd_addr		0xE207
.define 2 data_ptr   	0x5000
.define 2 stack_head	0x4000  


LDA %SP, H, stack_head[1]
LDA %SP, L, stack_head[0]
LDA %DP, H, data_ptr[1]
LDA %DP, L, data_ptr[0]
LDA %BP, H, #0x12
LDA %BP, L, #0x34
LDR R1, #4             // Recursion Depth
SDW R1, %DP, #0

OUT '\n'
OUT 'S'
OUT 'P'
OUT ':'
MAG R0, %SP, H
OUT R0, I
MAG R0, %SP, L
OUT R0, I
OUT ' '
OUT 'B'
OUT 'P'
OUT ':'
MAG R0, %BP, H
OUT R0, I
MAG R0, %BP, L
OUT R0, I
OUT '\n'
MAA %BP, %SP, #0
OUT 'B'
OUT 'P'
OUT ':'
MAG R0, %BP, H
OUT R0, I
MAG R0, %BP, L
OUT R0, I

LDR R0 #0x01
PUSH R0
LDR R0 #0x02
PUSH R0
LDR R0 #0x03
PUSH R0
LDR R0 #0x04
PUSH R0
LDR R0 #0x05
PUSH R0
LDR R0 #0x06
PUSH R0
LDR R0 #0x07
PUSH R0
LDR R0 #0x08
PUSH R0 
LDR R0 #0x09
PUSH R0 
LDR R0 #0x0a
PUSH R0 

OUT '\n'
OUT 'S'
OUT 'T'
OUT ':'
LDW R1, %BP, #-1
OUT R1, I
LDW R1, %BP, #-2
OUT R1, I
LDW R1, %BP, #-3
OUT R1, I
LDW R1, %BP, #-4
OUT R1, I
LDW R1, %BP, #-5
OUT R1, I
LDW R1, %BP, #-6
OUT R1, I
LDW R1, %BP, #-7
OUT R1, I
LDW R1, %BP, #-8
OUT R1, I
LDW R1, %BP, #-9
OUT R1, I
LDW R1, %BP, #-10
OUT R1, I
OUT '\n'

OUT 'S'
OUT 'P'
OUT ':'
MAG R0, %SP, H
OUT R0, I
MAG R0, %SP, L
OUT R0, I
OUT ' '
OUT 'B'
OUT 'P'
OUT ':'
MAG R0, %BP, H
OUT R0, I
MAG R0, %BP, L
OUT R0, I
OUT '\n'
MAA %DP, %SP, #0
LDW R1, %DP, #0
OUT R1, I
LDW R1, %DP, #1
OUT R1, I
LDW R1, %DP, #2
OUT R1, I
LDW R1, %DP, #3
OUT R1, I
LDW R1, %DP, #4
OUT R1, I
LDW R1, %DP, #5
OUT R1, I
LDW R1, %DP, #6
OUT R1, I
LDW R1, %DP, #7
OUT R1, I
LDW R1, %DP, #8
OUT R1, I
LDW R1, %DP, #9
OUT R1, I
OUT '\n'
POP R0
POP R0
POP R0
POP R0
POP R0
POP R0
POP R0
POP R0
POP R0
POP R0

OUT 'S'
OUT 'P'
OUT ':'
MAG R0, %SP, H
OUT R0, I
MAG R0, %SP, L
OUT R0, I
OUT ' '
OUT 'B'
OUT 'P'
OUT ':'
MAG R0, %BP, H
OUT R0, I
MAG R0, %BP, L
OUT R0, I
OUT '\n'
MAG R0, %PC, H
PUSH R0
MAG R0, %PC, L
PUSH R0

CALL :list_regs
OUT '\n'
OUT 'B'
OUT 'a'
OUT 'c'
OUT 'k'

OUT '\n'
OUT 'S'
OUT 'P'
OUT ':'
MAG R0, %SP, H
OUT R0, I
MAG R0, %SP, L
OUT R0, I
OUT ' '
OUT 'B'
OUT 'P'
OUT ':'
MAG R0, %BP, H
OUT R0, I
MAG R0, %BP, L
OUT R0, I
OUT '\n'
BRK


:list_regs
OUT '\n'
OUT 'R'
OUT 'e'
OUT 'g'
OUT 's'
OUT '\n'
OUT 'D'
OUT 'P'
OUT ':'
MAG R0, %DP, H
OUT R0, I
MAG R0, %DP, L
OUT R0, I
OUT '\n'

LDA %DP, H, data_ptr[1]
LDA %DP, L, data_ptr[0]
LDW R1, %DP, #0 

OUT 'R'
OUT 'e'
OUT 'c'
OUT ' '
OUT R1, I
OUT '\n'
DEC R1
BNE :cont
BRK
RTS
:cont
SDW R1, %DP, #0 

OUT 'S'
OUT 'P'
OUT ':'
MAG R0, %SP, H
OUT R0, I
MAG R0, %SP, L
OUT R0, I
OUT ' '
OUT 'B'
OUT 'P'
OUT ':'
MAG R0, %BP, H
OUT R0, I
MAG R0, %BP, L
OUT R0, I


OUT '\n'
OUT 'S'
OUT 'T'
OUT ':'

LDW R1, %BP, #-1
OUT R1, I
LDW R1, %BP, #-2
OUT R1, I
LDW R1, %BP, #-3
OUT R1, I
LDW R1, %BP, #-4
OUT R1, I
LDW R1, %BP, #-5
OUT R1, I
LDW R1, %BP, #-6
OUT R1, I
LDW R1, %BP, #-7
OUT R1, I
LDW R1, %BP, #-8
OUT R1, I
LDW R1, %BP, #-9
OUT R1, I
LDW R1, %BP, #-10
OUT R1, I
OUT '\n'
CALL :list_regs
RTS 



