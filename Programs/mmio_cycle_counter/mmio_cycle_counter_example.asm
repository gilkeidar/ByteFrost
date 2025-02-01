//	MMIO Cycle Counter Example
//	This program tests the first MMIO device added to the ByteFrost: a 32-bit
//	cycle counter! Its interface is as follows: (device1)
//	Address		R/W Access?		Effect
//	0xE200		W				Reset 32-bit counter (to 0)
//	0xE201		W				Latch 32-bit counter (snapshot current value)
//	0xE202		R				Read counter byte 0 (LSB)
//	0xE203		R				Read counter byte 1
//	0xE204		R				Read counter byte 2
//	0xE205		R				Read counter byte 3 (MSB)
//	(Note that the counter value is accessed as a little-endian integer)
//	===============================================
//	This program tests the counter in the following way:
//	0.	Setup.
//	1.	Reset the counter.
//	2.	Snapshot the current counter value and print it. x 3

.define	1 NEW_LINE	 	0x10
.define 1 SPACE			0x20
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
.define 1 COLON				0x3a
.define 1 LEFT_PAR			0x28
.define	1 RIGHT_PAR			0x29
.define 1 COMMA				0x2c
.define	1 PERIOD			0x2e
.define 1 ASTERISK			0x2a
.define 1 _x				0x78
.define 1 EQUALS			0x3d

.define	2 device1			0xE200

//	0.	Setup.
LSP	%HDP, device1[1]

//	1.	Reset the counter. (Write to 0xE200 - actual value written ignored.)
OUT _R, A		//	Print 'R'
OUT _E, A		//	Print 'E'
OUT _S, A		//	Print 'S'
OUT _E, A		//	Print 'E'
OUT _T, A		//	Print 'T'
OUT SPACE, A	//	Print ' '
OUT _C, A		//	Print 'C'
OUT _O, A		//	Print 'O'
OUT _U, A		//	Print 'U'
OUT _N, A		//	Print 'N'
OUT _T, A		//	Print 'T'
OUT _E, A		//	Print 'E'
OUT _R, A		//	Print 'R'
OUT PERIOD, A	//	Print '.'
OUT NEW_LINE, A	//	Print '\n'
SMA R0, #0x00

//	2.	Snapshot the current counter value and print it 3 times.
//	R1 = 3
//	while (R1 != 0) {
//		Snapshot current counter value.
//		Print the 32-bit counter value. (Counter: [8-digit counter val]\n)
//		R1--
//	}
LDR R1, #3			//	R1 = 3
:while_loop
	BEQ :after_loop
	SMA R0, #0x01	//	Snapshot the current counter value (Write to 0xE201)
	//	Print 32-bit counter value. ("Counter: [8-digit counter val]\n")
	OUT _C, A		//	Print 'C'
	OUT _O, A		//	Print 'O'
	OUT _U, A		//	Print 'U'
	OUT _N, A		//	Print 'N'
	OUT _T, A		//	Print 'T'
	OUT _E, A		//	Print 'E'
	OUT _R, A		//	Print 'R'
	OUT COLON, A	//	Print ':'
	OUT SPACE, A	//	Print ' '
	LMA R2, #0x05	//	Read MSB and print it in hex mode.
	OUT R2, I
	LMA R2, #0x04	//	Read 3rd byte and print it in hex mode.
	OUT R2, I
	LMA R2, #0x03	//	Read 2nd byte and print it in hex mode.
	OUT R2, I
	LMA R2, #0x02	//	Read LSB and print it in hex mode.
	OUT R2, I
	DEC R1			//	R1--
	BEQ :after_loop	//	If R1 == 0, don't print new-line again.
	OUT NEW_LINE, A	//	Print '\n'
	JMP :while_loop
:after_loop
BRK
