//	Read A Sector from Disk into a RAM Page Test
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
//	1.	Send a command to the SD Card Arduino to read a sector (256 bytes) from
//		the ByteFrost disk (file) into a page (256 bytes) of the ByteFrost RAM.
//	2.	Print the contents of the page that was written to.
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
.define	2 device1			0xE200
.define	2 device2			0xE400
.define	2 device3			0xE600
.define	2 device4			0xE800
.define	2 device5			0xEA00
.define	2 device6			0xEC00
.define	2 device7			0xEE00

//	1.	Send a command to the SD Card Arduino to read a sector (256 bytes) from
//		the ByteFrost disk (file) into a page (256 bytes) of the ByteFrost RAM.

//		1.	Print "*(0xE000)"
LSP %HDP, sdcard[1]

OUT ASTERISK, A
OUT LEFT_PAR, A
OUT #0, A
OUT _x, A
OUT sdcard[1], I
OUT sdcard[0], I
OUT RIGHT_PAR, A

//		2. Set SDcard command
//  Byte 0: Page
//  Byte 1: Sector High ( 3 lsb)
//  Byte 2: Sector Low
//  Byte 3: Go          (how do we tell to read or write? - part of sector high
//							address (byte 1))
//	SD Card Command format (16 bit command):
//	15	14	13	12	11	10-0
//	R/W	?	?	?	?	Sector ID
LDR R0, #0
LDR R1, #0x17 
SMR R1, R0 // Set Page to be 0x17
INC R0
//LDR R1, #0x03  
LDR R1, #0x00
SMR R1, R0 // Set Sector High = 0x00 (was 0x03)
INC R0
//LDR R1, #0x45  
LDR R1, #0x00
SMR R1, R0 // Set Sector Low = 0x00 (was 0x45)
INC R0
SMR R1, R0 // GO (R1 is ignored)

//	At this point, the ByteFrost will be stopped while the SD Card Arduino
//	writes sector 0 to page 0x17.
OUT NEW_LINE, A

//	2.	Print the contents of the page that was written to.
LSP %HDP, #0x17		//	Set High Data Pointer to page 0x17

//	R0 = 0;
//	while (R0 <= 255) {
//		Print (*(HDP, R0))
//		R0++
//	}

LDR R0, #0
LDR R3, #20
:while
SUB R1, R0, R3		//	R1 = R0 - 20
BPL :after_while	//	while (R0 <= 20) {
LMR R2, R0			//	R2 = *(HDP, R0)
OUT R2, A			//	Print R2
INC R0				//	R0++
JMP :while			//	}

:after_while		
BRK
