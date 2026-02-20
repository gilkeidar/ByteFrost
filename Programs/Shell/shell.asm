// ByteFrostSHELL

//.define 1 PROMPT 0x24   //  '$'
.define 1 PROMPT 0x5d     // ']'

OUT '\n'   // Space from previous run on the minicom
OUT '\n'
OUT 'S'
OUT 'H'
OUT 'E'
OUT 'L'
OUT 'L'
OUT ' '
OUT 'v'
OUT '1'
OUT '.'
OUT '0'
OUT '5'
OUT '\n'

.define 2 kbd_addr	0xE207
.define 2 command_line 0xDF00
.define 1 max_command_length 255

:shell_loop
OUT PROMPT, A
OUT ' '

LDA %BP, H, command_line[1]
LDA %BP, L, command_line[0]

LDR R3, #0						// Empty command line
SDW R3, %BP, #0					// Update Pointer value in memory

:poll_character
LDA %DP, H, kbd_addr[1]
LDA %DP, L, kbd_addr[0]
LDA %BP, H, command_line[1]
LDA %BP, L, command_line[0]
// The command line is: 1 byte length, 10 bytes command 

LDW R0, %DP, #0  
TST R0, #0 // Was there any character?

BEQ  :poll_character
LDR R1, #13 // 'Return'
TST R0, R1 
BEQ :dispatcher
OUT R0, A
// Add character to the command line
LDW R3, %BP, #0 				// Current Command length
LDR R1, max_command_length
TST R3, R1 						// Has Command reached max_command_legth?    
BEQ :poll_character                 // Ignore the char after max length   
INC R3							// Move pointer to next char
SDW R3, %BP, #0					// Update Pointer value in memory
MGA %BP, L, R3 					// Move BP to point next char
SDW R0, %BP, #0					// store R0 as the next char
JMP  :poll_character

:dispatcher
OUT '\n'
// Test - Echo command
OUT 'C'
OUT 'M'
OUT 'D'
OUT ':'
OUT ' '
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
OUT '\n'
LDA %BP, L, command_line[0]		// Restore BP
LDR R3, #0
SDW R3, %BP, #0					// Update Pointer value in memory
JMP  :shell_loop