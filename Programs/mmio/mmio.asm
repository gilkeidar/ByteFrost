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

.define	2 sdcard 			0xE000
.define	2 counter_trng		0xE200
.define	2 device2			0xE400
.define	2 device3			0xE600
.define	2 device4			0xE800
.define	2 device5			0xEA00
.define	2 device6			0xEC00
.define	2 device7			0xEE00

//	1.	Read address 0xE000, 0xE200, ..., 0xEE00


//	1.	Read 0xE000
.define	1 page_from_SD		0x44 // RAM Pages Range: 20..DF
//		1.	Print "*(0xE000) ="
LDA %DP, H, sdcard[1]
LDA %DP, L, sdcard[0]
LDA %BP, H, page_from_SD
LDA %BP, L, #0x00

OUT NEW_LINE, A // Useful on the remote terminal
OUT NEW_LINE, A
OUT ASTERISK, A
OUT LEFT_PAR, A
OUT #0, A
OUT _x, A
OUT sdcard[1], I
OUT sdcard[0], I
OUT RIGHT_PAR, A
OUT SPACE, A
OUT EQUALS, A
OUT SPACE, A
 
LDR R0, #0x5A
SDW R0, %BP, #1
SDW R0, %BP, #0x7F
           // Now check for completion status...
LDW R2, %BP, #1
OUT R2, I
LDW R2, %BP, #0x7F
OUT R2, I
OUT SPACE, A
//		2. Set SDcard command
//  Byte 0: Page
//  Byte 1: R/W, Sector High (bit 7 - R/W; Sector High bits 2:0)
//  Byte 2: Sector Low
//  Byte 3: Go          (how do we tell to read or write?)

LDR R1, page_from_SD 
SDW R1, %DP, #0  	// Set Page to be 0x17
LDR R1, #0x03  
SDW R1, %DP, #1  	// Set Sector High = 0x03 and operation to READ
LDR R1, #0x45 
SDW R1, %DP, #2  	// Set Sector Low = 0x45 (0x345 = 837)
SDW R1, %DP, #3  	// GO (R1 is ignored)
      
           // Now check for completion status...
LDW R2, %BP, #1
OUT R2, I
LDW R2, %BP, #0x7F
OUT R2, I
//		3. Print R2, R3 (Should be 5A5A)

///////////////////////////////////////
//	2.	Read 0xE200
// This MMIO serves both Counter and TRNG
// 
//	32bit Counter:
//	Address		R/W Access?		Effect
//  -------     -----------     ------
//	0xE200		W				Reset 32-bit counter (to 0)
//	0xE201		W				Latch 32-bit counter (snapshot current value)
//	0xE202		R				Read counter byte 0 (LSB)
//	0xE203		R				Read counter byte 1
//	0xE204		R				Read counter byte 2
//	0xE205		R				Read counter byte 3 (MSB)
//	(Note that the counter value is accessed as a little-endian integer)
//
// 
OUT NEW_LINE, A

//		1.	Print "*(0xE200) ="
LDA %DP, H, counter_trng[1]
LDA %DP, L, counter_trng[0]

OUT ASTERISK, A
OUT LEFT_PAR, A
OUT #0, A
OUT _x, A
OUT counter_trng[1], I
OUT counter_trng[0], I
OUT RIGHT_PAR, A
OUT SPACE, A
OUT _C, A
OUT _T, A
OUT _R, A
OUT EQUALS, A
OUT SPACE, A

SDW R0, %DP, #0     // Reset Counter

NOP
NOP
NOP
NOP
NOP   // Time 5 NOPS. Counter = 5 * 2 + 4 = 14 (0xE)

SDW R0, %DP, #1  // Latch Counter
// LDW R0, %DP, #5  // Print Byte4
// OUT R0, I
LDW R0, %DP, #4  // Print Byte3
OUT R0, I
LDW R1, %DP, #3  // Print Byte2
OUT R1, I
LDW R2, %DP, #2  // Print Byte1
OUT R2, I

OUT NEW_LINE, A
OUT ASTERISK, A
OUT LEFT_PAR, A
OUT #0, A
OUT _x, A
OUT counter_trng[1], I
OUT counter_trng[0], I
OUT RIGHT_PAR, A
OUT SPACE, A
OUT _R, A
OUT _N, A
OUT _G, A
OUT EQUALS, A
OUT SPACE, A

// TRNG
// 		0xE206		Random Data
LDW R3, %DP, #6  // 0xE206		Random Data
OUT R3, I
LDW R3, %DP, #6  // 0xE206		Random Data
OUT R3, I
LDW R3, %DP, #6  // 0xE206		Random Data
OUT R3, I
LDW R3, %DP, #6  // 0xE206		Random Data
OUT R3, I
LDW R3, %DP, #6  // 0xE206		Random Data
OUT R3, I

//	3.	Read 0xE400
OUT NEW_LINE, A
////////////////////////////////////////////
//		1.	Print "*(0xE400) ="

LDA %DP, H, device2[1]
LDA %DP, L, device2[0]

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

LDW R0, %DP, #2

//		3. Print R0
OUT R0, I
////////////////////////////////////////////
//	4.	Read 0xE600
OUT NEW_LINE, A

//		1.	Print "*(0xE600) ="

LDA %DP, H, device3[1]
LDA %DP, L, device3[0]

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
LDW R0, %DP, #2

//		3. Print R0
OUT R0, I

//	2.	Read 0xE800
OUT NEW_LINE, A
/////////////////////////////////////////////////
//		1.	Print "*(0xE800) ="

LDA %DP, H, device4[1]
LDA %DP, L, device4[0]

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
LDW R0, %DP, #2

//		3. Print R0
OUT R0, I

//	2.	Read 0xEA00
OUT NEW_LINE, A
//////////////////////////////////////////
//		1.	Print "*(0xEA00) ="
 
LDA %DP, H, device5[1]
LDA %DP, L, device5[0]

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
LDW R0, %DP, #2

//		3. Print R0
OUT R0, I

//	2.	Read 0xE200
OUT NEW_LINE, A
//////////////////////////////////////////
//		1.	Print "*(0xEC00) ="

LDA %DP, H, device6[1]
LDA %DP, L, device6[0]

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
LDW R0, %DP, #2

//		3. Print R0
OUT R0, I
/////////////////////////////////////////////
//	2.	Read 0xEE00
OUT NEW_LINE, A

//		1.	Print "*(0xEE00) ="

LDA %DP, H, device7[1]
LDA %DP, L, device7[0]

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
LDW R0, %DP, #2

//		3. Print R0
OUT R0, I

//////////////REPEAT COUNTER and TRNG //////////////////////
//		1.	Print "*(0xE200) ="
LDA %DP, H, counter_trng[1]
LDA %DP, L, counter_trng[0]

OUT NEW_LINE, A
OUT ASTERISK, A
OUT LEFT_PAR, A
OUT #0, A
OUT _x, A
OUT counter_trng[1], I
OUT counter_trng[0], I
OUT RIGHT_PAR, A
OUT SPACE, A
OUT _C, A
OUT _T, A
OUT _R, A
OUT EQUALS, A
OUT SPACE, A

// Dont reset this time .. SDW R0, %DP, #0     // Reset Counter

NOP
NOP
NOP
NOP
NOP   // Time 5 NOPS. Counter = 5 * 2 + 4 = 14 (0xE)

SDW R0, %DP, #1  // Latch Counter
// LDW R0, %DP, #5  // Print Byte4
// OUT R0, I
LDW R0, %DP, #4  // Print Byte3
OUT R0, I
LDW R1, %DP, #3  // Print Byte2
OUT R1, I
LDW R2, %DP, #2  // Print Byte1
OUT R2, I

OUT NEW_LINE, A
OUT ASTERISK, A
OUT LEFT_PAR, A
OUT #0, A
OUT _x, A
OUT counter_trng[1], I
OUT counter_trng[0], I
OUT RIGHT_PAR, A
OUT SPACE, A
OUT _R, A
OUT _N, A
OUT _G, A
OUT EQUALS, A
OUT SPACE, A

// TRNG
// 		0xE206		Random Data
LDW R3, %DP, #6  // 0xE206		Random Data
OUT R3, I
LDW R3, %DP, #6  // 0xE206		Random Data
OUT R3, I
LDW R3, %DP, #6  // 0xE206		Random Data
OUT R3, I
LDW R3, %DP, #6  // 0xE206		Random Data
OUT R3, I
LDW R3, %DP, #6  // 0xE206		Random Data
OUT R3, I

BRK
