//	MMIO Test
//	MMIO addresses are in the last of 8 sections that compose the ByteFrost
//	memory address space (hence, they begin at address 0xE000). Therefore, the 
//	top 8 KB are reserved for MMIO, and these 8 KB are further subdivided into
//	16 sections, each of which is to be used by one device. This means that the
//	ByteFrost can support 16 different I/O devices(!)
//	Hence, a ByteFrost memory address (16-bit) is broken up as follows:
//		[3 bits section][13 bits section offset]
//		If the section bits are 111, then the address is an MMIO address, which
//		is broken up as follows:
//		[111][4 bits device ID][9 bits device offset]
//
//	This program does the following:
//	1.	Access address 0xE000.
//	2.	Print the byte of data from this address (currently, there is no MMIO
//		register at that address, no entity will write to the data bus - given 
//		that the bus has pull-down resistors, the expected value to be printed
//		is therefore 0.

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
.define 1 ASTERISK			0x2a
.define 1 _x				0x78
.define 1 EQUALS			0x3d

.define	2 device0			0xE000
.define	2 device1			0xE200
.define	2 device2			0xE400
.define	2 device3			0xE600
.define	2 device4			0xE800
.define	2 device5			0xEA00
.define	2 device6			0xEC00
.define	2 device7			0xEE00

//	1.	Read address 0xE000, 0xE200, ..., 0xEE00


//	1.	Read 0xE000

//		1.	Print "*(0xE000) ="
LSP %HDP, device0[1]

OUT ASTERISK, A
OUT LEFT_PAR, A
OUT #0, A
OUT _x, A
OUT device0[1], I
OUT device0[0], I
OUT RIGHT_PAR, A
OUT SPACE, A
OUT EQUALS, A
OUT SPACE, A

//		2. R0 = *(0xE000)
LMA R0, device0[0]

//		3. Print R0
OUT R0, I

//	2.	Read 0xE200
OUT NEW_LINE, A

//		1.	Print "*(0xE200) ="
LSP %HDP, device1[1]

OUT ASTERISK, A
OUT LEFT_PAR, A
OUT #0, A
OUT _x, A
OUT device1[1], I
OUT device1[0], I
OUT RIGHT_PAR, A
OUT SPACE, A
OUT EQUALS, A
OUT SPACE, A

//		2. R0 = *(0xE200)
LMA R0, device1[0]

//		3. Print R0
OUT R0, I

//	3.	Read 0xE400
OUT NEW_LINE, A

//		1.	Print "*(0xE400) ="
LSP %HDP, device2[1]

OUT ASTERISK, A
OUT LEFT_PAR, A
OUT #0, A
OUT _x, A
OUT device2[1], I
OUT device2[0], I
OUT RIGHT_PAR, A
OUT SPACE, A
OUT EQUALS, A
OUT SPACE, A

//		2. R0 = *(0xE400)
LMA R0, device2[0]

//		3. Print R0
OUT R0, I

//	4.	Read 0xE600
OUT NEW_LINE, A

//		1.	Print "*(0xE600) ="
LSP %HDP, device3[1]

OUT ASTERISK, A
OUT LEFT_PAR, A
OUT #0, A
OUT _x, A
OUT device3[1], I
OUT device3[0], I
OUT RIGHT_PAR, A
OUT SPACE, A
OUT EQUALS, A
OUT SPACE, A

//		2. R0 = *(0xE600)
LMA R0, device3[0]

//		3. Print R0
OUT R0, I

//	2.	Read 0xE800
OUT NEW_LINE, A

//		1.	Print "*(0xE800) ="
LSP %HDP, device4[1]

OUT ASTERISK, A
OUT LEFT_PAR, A
OUT #0, A
OUT _x, A
OUT device4[1], I
OUT device4[0], I
OUT RIGHT_PAR, A
OUT SPACE, A
OUT EQUALS, A
OUT SPACE, A

//		2. R0 = *(0xE800)
LMA R0, device4[0]

//		3. Print R0
OUT R0, I

//	2.	Read 0xEA00
OUT NEW_LINE, A

//		1.	Print "*(0xEA00) ="
LSP %HDP, device5[1]

OUT ASTERISK, A
OUT LEFT_PAR, A
OUT #0, A
OUT _x, A
OUT device5[1], I
OUT device5[0], I
OUT RIGHT_PAR, A
OUT SPACE, A
OUT EQUALS, A
OUT SPACE, A

//		2. R0 = *(0xEA00)
LMA R0, device5[0]

//		3. Print R0
OUT R0, I

//	2.	Read 0xE200
OUT NEW_LINE, A

//		1.	Print "*(0xEC00) ="
LSP %HDP, device6[1]

OUT ASTERISK, A
OUT LEFT_PAR, A
OUT #0, A
OUT _x, A
OUT device6[1], I
OUT device6[0], I
OUT RIGHT_PAR, A
OUT SPACE, A
OUT EQUALS, A
OUT SPACE, A

//		2. R0 = *(0xEC00)
LMA R0, device6[0]

//		3. Print R0
OUT R0, I

//	2.	Read 0xEE00
OUT NEW_LINE, A

//		1.	Print "*(0xEE00) ="
LSP %HDP, device7[1]

OUT ASTERISK, A
OUT LEFT_PAR, A
OUT #0, A
OUT _x, A
OUT device7[1], I
OUT device7[0], I
OUT RIGHT_PAR, A
OUT SPACE, A
OUT EQUALS, A
OUT SPACE, A

//		2. R0 = *(0xEE00)
LMA R0, device7[0]

//		3. Print R0
OUT R0, I


BRK
