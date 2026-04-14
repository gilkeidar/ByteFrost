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
OUT '7'
OUT '\n'

.define 2 kbd_addr		0xE207
.define 2 command_line 	0xDF00
.define 2 stack_head	0x4000  
.define 1 max_command_length 255

LDA %SP, H, stack_head[1]
LDA %SP, L, stack_head[0]

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

LDA %BP, L, command_line[0]		// BP -> command_line.length
LDW R3, %BP, #0 				// R3 = command_length
TST R3, #0                      // if (length==0) break
BEQ :end_echo					// Just <ENTER> without command
LDR R0, command_line[0]			// i = *command_line
INC R0							// i++  // 	command_line.data[0]
MGA %BP, L, R0 					// BP -> *command_line.data[0]
LDW R1, %BP, #0					// R1 = command_line.data[0]
                    // Start the dispach
					// switch(R1 command) 
LDR R2, #0x4C                   // Case 'L' - List
TST R1, R2
BEQ :list_mem
LDR R2, #0x6C                   // Case 'l' - List
TST R1, R2
BEQ :list_mem

LDR R2, #0x52                   // Case 'R' - Registers
TST R1, R2
BNE :test_r
CALL :list_regs
JMP :end_echo

:test_r
LDR R2, #0x72                   // Case 'r' - Registers
TST R1, R2
BNE :next_r
CALL :list_regs
JMP :end_echo
:next_r

:unknown_command_section        // default: 
OUT '\n'
// Test - Echo command
OUT 'C'
OUT 'M'
OUT 'D'
OUT '?'
OUT ' '
LDR R0, command_line[0]			// i = *command_line
INC R0							// i++  // 	command_line.data[0]
:unknown_command				// do {
MGA %BP, L, R0 					// 		BP -> command_line[i]
LDW R1, %BP, #0					// 		R1 = command_line[i]
OUT R1, A						// 		Print R1
INC R0							// 		i++	
TST R3, R0						// } while (i < command_length)
BPL :unknown_command

:end_echo
OUT '\n'
LDA %BP, L, command_line[0]		// Restore BP
LDR R3, #0
SDW R3, %BP, #0					// Update Pointer value in memory
JMP  :shell_loop


///////////////////////////////////////////////////////////
// list_registers - shell command 'R' 
// lists all registers
//
///////////////////////////////////////////////////////////

:list_regs
OUT '\n'
OUT 'R'
OUT 'e'
OUT 'g'
OUT 's'
OUT '\n'
OUT 'R'
OUT '0'
OUT ':'
OUT R0, I
OUT ' '
OUT 'R'
OUT '1'
OUT ':'
OUT R1, I
OUT '\n'
OUT 'R'
OUT '2'
OUT ':'
OUT R2, I
OUT ' '
OUT 'R'
OUT '3'
OUT ':'
OUT R3, I
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
OUT 'D'
OUT 'P'
OUT ':'
MAG R0, %DP, H
OUT R0, I
MAG R0, %DP, L
OUT R0, I
OUT ' '
OUT 'P'
OUT 'C'
OUT ':'
MAG R0, %PC, H
OUT R0, I
MAG R0, %PC, L
OUT R0, I
OUT '\n'
OUT 'R'
OUT 'T'
OUT ':'
LDW R0, %SP, #-1
OUT R0, I
LDW R0, %SP, #0
OUT R0, I
BRK
RTS
 

///////////////////////////////////////////////////////////
// list_mem - shell command 'L1234' 
// lists 16 bytes of memory starting from the given address
//
///////////////////////////////////////////////////////////

:list_mem
OUT '\n'
OUT 'L'
OUT 'i'
OUT 's'
OUT 't'
OUT ' '
TST R3, #5                      // if (length!=5) break
BNE :unknown_command_section					 
LDR R0, command_line[0]			// i = *command_line
INC R0							// i++  // 	command_line.data[0]
INC R0							// i++  // 	command_line.data[1]
:process_address				// do {
MGA %BP, L, R0 					// 		BP -> command_line[i]
LDW R1, %BP, #0					// 		R1 = command_line[i]
OUT R1, A						// 		Print R1
PUSH R1       					// Put ASCII on stack
INC R0							// 		i++	
TST R3, R0						// } while (i < command_length)
BPL :process_address
CALL :atoi_4                    // DP <- :atoi_4/2; --SP <- PC; PC = DP;
POP R1    						// Empty stack
POP R1
POP R1
POP R1


JMP :end_echo

///////////////////////////////////////////////////////////
// atoi_4 gets number of 4 ASCII in stack and returns  
// 2 byte hexadecimal in stack (caller pop twice) 
//
///////////////////////////////////////////////////////////
:atoi_4
OUT '\n'
OUT 'A'
OUT '2'
OUT 'I'
OUT '4'
OUT ' '
CALL :list_regs
MAA %BP, %SP, #0
CALL :list_regs
OUT '\n'
LDW R1, %BP, #0
OUT R1, I
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
OUT '\n'
MAG R1, %SP, H
OUT R1, I
MAG R1, %SP, L
OUT R1, I
OUT '\n'
RTS

