// ByteFrostSHELL

.define	1 NEW_LINE	 		0x10
.define 1 SPACE				0x20
.define 1 LEFT_PAR			0x28
.define	1 RIGHT_PAR			0x29
.define 1 COMMA				0x2c
.define 1 ASTERISK			0x2a
.define 1 DASH				0x2d
.define 1 DOT				0x2e
.define 1 COLON				0x3a
.define 1 EQUALS			0x3d

.define	1 _A				0x41
.define	1 _B				0x42
.define	1 _C				0x43
.define	1 _D				0x44
.define	1 _E				0x45
.define	1 _F				0x46
.define	1 _G				0x47
.define	1 _H				0x48
.define	1 _I				0x49
.define	1 _J				0x4a
.define	1 _K				0x4b
.define	1 _L				0x4c
.define	1 _M				0x4d
.define	1 _N				0x4e
.define	1 _O				0x4f
.define	1 _P				0x50
.define	1 _Q				0x51
.define	1 _R				0x52
.define	1 _S				0x53
.define	1 _T				0x54
.define	1 _U				0x55
.define	1 _V				0x56
.define	1 _W				0x57
.define	1 _X				0x58
.define	1 _Y				0x59
.define	1 _Z				0x5a

.define 1 _l				0x6c
.define 1 _m				0x6d
.define 1 _n				0x6e
.define 1 _o				0x6f
.define 1 _p				0x70
.define 1 _q				0x71
.define 1 _r				0x72
.define 1 _s				0x73
.define 1 _t				0x74
.define 1 _u				0x75
.define 1 _v				0x76
.define 1 _w				0x77
.define 1 _x				0x78
.define 1 _y				0x79
.define 1 _z				0x7a

OUT NEW_LINE, A   // Space from previous run on the minicom
OUT NEW_LINE, A
OUT _S, A
OUT _H, A
OUT _E, A
OUT _L, A
OUT _L, A
OUT SPACE, A
OUT _v, A
OUT #1, A
OUT DOT, A
OUT #0, A
OUT #3, A

OUT NEW_LINE, A


.define 2 kbd_addr	0xE207
.define 2 command_line 0xDF00
.define 1 max_command_length 10

LDR R3, #0						// Empty command line
SDW R3, %BP, #0					// Update Pointer value in memory

:shell_loop

LDA %DP, H, kbd_addr[1]
LDA %DP, L, kbd_addr[0]
LDA %BP, H, command_line[1]
LDA %BP, L, command_line[0]
// The command line is: 1 byte length, 10 bytes command 

LDW R0, %DP, #0  
TST R0, #0 // Was there any charecter?

BEQ  :shell_loop
LDR R1, #13 // 'Return'
TST R0, R1 
BEQ :dispatcher
OUT R0, A
// Add charecter to the command line
LDW R3, %BP, #0 				// Current Command length
LDR R1, max_command_length
TST R3, R1 						// Has Command reached max_command_legth?    
BEQ :shell_loop                 // Ignore the char after max length   
INC R3							// Move pointer to next char
SDW R3, %BP, #0					// Update Pointer value in memory
MGA %BP, L, R3 					// Move BP to point next char
SDW R0, %BP, #0					// store R0 as the next char
JMP  :shell_loop

:dispatcher
OUT NEW_LINE, A
// Test - Echo command
OUT _C, A
OUT _M, A
OUT _D, A
OUT COLON, A
LDA %BP, L, command_line[0]		// Restore BP
LDW R3, %BP, #0 				// Load Current Command length
TST R3, #0
BEQ :end_echo

LDR R0, #1
:echo_command
MGA %BP, L, R0 	
LDW R1, %BP, #0
OUT R1, A
INC R0
TST R3, R0
BPL :echo_command

:end_echo
OUT NEW_LINE, A
LDA %BP, L, command_line[0]		// Restore BP
LDR R3, #0
SDW R3, %BP, #0					// Update Pointer value in memory
JMP  :shell_loop

