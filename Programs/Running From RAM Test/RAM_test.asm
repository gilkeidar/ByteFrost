//	This program runs code from the ROM, copies a function to the RAM, then
//	jumps to that function in the RAM.
//	Note: Each page is 256 bytes long
//	Preamble - set dummy PC high byte register and stack page
LSP %PC, #0x00		//	Dummy PC starts at page 0 (in ROM)
LSP %SP, #0xDF		//	Stack page is 223 (last RAM page)
:main	//	main() runs in the ROM
	//	1.	Print "Running from ROM\n"
	OUT #0x52, A	//	Print 'R'
	OUT #0x75, A	//	Print 'u'
	OUT #0x6e, A	//	Print 'n'
	OUT #0x6e, A	//	Print 'n'
	OUT #0x69, A	//	Pring 'i'
	OUT #0x6e, A	//	Print 'n'
	OUT #0x67, A	//	Print 'g'
	OUT #0x20, A	//	Print ' '
	OUT #0x66, A	//	Print 'f'
	OUT #0x72, A	//	Print 'r'
	OUT #0x6f, A	//	Print 'o'
	OUT #0x6d, A	//	Print 'm'
	OUT #0x20, A	//	Print ' '
	OUT #0x52, A	//	Print 'R'
	OUT #0x4f, A	//	Print 'O'
	OUT #0x4d, A	//	Print 'M'
	OUT #0x10, A	//	Print '\n'
	//	2.	Copy ram_function() to first RAM page
	LSP %DP, #0x20	//	Set DP to point to first RAM page (to copy ram_function
					//	from ROM to RAM)
	LDR R0, :ram_function	//	Load R0 with ram_function address
	DEC R0					//	Ensure R0 points to correct address (weird assembler shenanigans)
	LDR R1, #0		//	Load R1 with starting address in RAM page
	LDR R2, #18		//	Length of copy (length of ram_function in instructions)
	ASL R2, #1		//	R2 = R2 << 1 (length of ram_function in bytes)
:while_loop
	TST R2				//	while (R2 != 0)
	BEQ :after_while	//	{
	LSP %DP, #0x00		//		DP points to first ROM page
	LMR R3, R0			//		R3 = *({0x00, R0})
	LSP %DP, #0x20		//		DP points to page 0x20 in RAM
	SMR R3, R1			//		*({0x20, R1}) = R3
	INC R0				//		R0++
	INC R1				//		R1++
	DEC R2				//		R2--
	JMP :while_loop		//	}
:after_while
	//	3.	Call ram_function()
	LSP %PC, #0x20	//	Set dummy PC high byte to #0x20 (first RAM page)
	LDR R0, #0x00	//	R0 = 0x00
	PUSH R0 		//	Push high byte of return address
	JSR	:ram_function			//	Call ram_function()
	//	4.	Print "Done"
	OUT #0x44, A	//	Print 'D'
	OUT #0x6f, A	//	Print 'o'
	OUT #0x6e, A	//	Print 'n'
	OUT #0x65, A	//	Print 'e'
	BRK
:ram_function
	//	ram_function() - stored in ROM then copied and run from RAM
	//	1. Print "Running from RAM\n"
	OUT #0x52, A	//	Print 'R'
	OUT #0x75, A	//	Print 'u'
	OUT #0x6e, A	//	Print 'n'
	OUT #0x6e, A	//	Print 'n'
	OUT #0x69, A	//	Pring 'i'
	OUT #0x6e, A	//	Print 'n'
	OUT #0x67, A	//	Print 'g'
	OUT #0x20, A	//	Print ' '
	OUT #0x66, A	//	Print 'f'
	OUT #0x72, A	//	Print 'r'
	OUT #0x6f, A	//	Print 'o'
	OUT #0x6d, A	//	Print 'm'
	OUT #0x20, A	//	Print ' '
	OUT #0x52, A	//	Print 'R'
	OUT #0x41, A	//	Print 'A'
	OUT #0x4d, A	//	Print 'M'
	OUT #0x10, A	//	Print '\n'
	//	2.	Return
	RTS 