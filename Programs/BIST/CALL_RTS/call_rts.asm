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
.define 2 data_ptr   	0x4455
.define 2 stack_head	0x4000  


LDA %SP, H, stack_head[1]
LDA %SP, L, stack_head[0]
LDA %DP, H, data_ptr[1]
LDA %DP, L, data_ptr[0]
LDA %BP, H, #0x66
LDA %BP, L, #0x77
LDR R1, #4             // Recursion Depth
SDW R1, %DP, #0
LDR R1, #0xE             // Recursion Depth
SDW R1, %BP, #0

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

OUT 'D'
OUT 'P'
OUT ':'
MAG R0, %DP, H
OUT R0, I
MAG R0, %DP, L
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

OUT 'B'
OUT 'P'
OUT '*'
BRK
LDW R1, %DP, #0
OUT R1, I
LDW R1, %BP, #0
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
 
OUT 'D'
OUT 'P'
OUT ':'
MAG R0, %DP, H
OUT R0, I
MAG R0, %DP, L
OUT R0, I
OUT '\n'

OUT 'D'
OUT 'P'
OUT '*'
LDW R1, %DP, #-1
OUT R1, I
LDW R1, %DP, #-2
OUT R1, I
LDW R1, %DP, #-3
OUT R1, I
LDW R1, %DP, #-4
OUT R1, I
LDW R1, %DP, #-5
OUT R1, I
LDW R1, %DP, #-6
OUT R1, I
LDW R1, %DP, #-7
OUT R1, I
LDW R1, %DP, #-8
OUT R1, I
LDW R1, %DP, #-9
OUT R1, I
LDW R1, %DP, #-10
OUT R1, I
OUT '\n'

OUT 'S'
OUT 'P'
OUT ':'
MAG R0, %SP, H
OUT R0, I
MAG R0, %SP, L
OUT R0, I
OUT '\n'

OUT 'S'
OUT 'P'
OUT '*'
LDW R1, %SP, #-1
OUT R1, I
LDW R1, %SP, #-2
OUT R1, I
LDW R1, %SP, #-3
OUT R1, I
LDW R1, %SP, #-4
OUT R1, I
LDW R1, %SP, #-5
OUT R1, I
LDW R1, %SP, #-6
OUT R1, I
LDW R1, %SP, #-7
OUT R1, I
LDW R1, %SP, #-8
OUT R1, I
LDW R1, %SP, #-9
OUT R1, I
LDW R1, %SP, #-10
OUT R1, I
OUT '\n'

// LDA %SP, H, stack_head[1]
// LDA %SP, L, stack_head[0]
MAA %SP, %SP, #6
LDA %DP, H, stack_head[1]
LDA %DP, L, stack_head[0]
MAA %DP, %DP, #-6
LDA %BP, H, stack_head[1]
LDA %BP, L, stack_head[0]


OUT '\n'
OUT 'B'
OUT 'P'
OUT ':'
MAG R0, %BP, H
OUT R0, I
MAG R0, %BP, L
OUT R0, I
OUT '\n'

OUT 'B'
OUT 'P'
OUT '*'
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
 
OUT 'D'
OUT 'P'
OUT ':'
MAG R0, %DP, H
OUT R0, I
MAG R0, %DP, L
OUT R0, I
OUT '\n'

OUT 'D'
OUT 'P'
OUT '*'
LDW R1, %DP, #-1
OUT R1, I
LDW R1, %DP, #-2
OUT R1, I
LDW R1, %DP, #-3
OUT R1, I
LDW R1, %DP, #-4
OUT R1, I
LDW R1, %DP, #-5
OUT R1, I
LDW R1, %DP, #-6
OUT R1, I
LDW R1, %DP, #-7
OUT R1, I
LDW R1, %DP, #-8
OUT R1, I
LDW R1, %DP, #-9
OUT R1, I
LDW R1, %DP, #-10
OUT R1, I
OUT '\n'

OUT 'S'
OUT 'P'
OUT ':'
MAG R0, %SP, H
OUT R0, I
MAG R0, %SP, L
OUT R0, I
OUT '\n'

OUT 'S'
OUT 'P'
OUT '*'
LDW R1, %SP, #-1
OUT R1, I
LDW R1, %SP, #-2
OUT R1, I
LDW R1, %SP, #-3
OUT R1, I
LDW R1, %SP, #-4
OUT R1, I
LDW R1, %SP, #-5
OUT R1, I
LDW R1, %SP, #-6
OUT R1, I
LDW R1, %SP, #-7
OUT R1, I
LDW R1, %SP, #-8
OUT R1, I
LDW R1, %SP, #-9
OUT R1, I
LDW R1, %SP, #-10
OUT R1, I
OUT '\n'
BRK



