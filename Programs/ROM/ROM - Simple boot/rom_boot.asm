//	ROM Boot Code
//	Assumes:
//		1.	A BFSv1 filesystem is in use and the ByteFrost disk file used on the
//			disk has its boot sector correctly set up with first mutable code.

//	This ROM Boot Code does the following:
//	1.	Load the boot sector from disk into RAM (lowest possible pages)
//		*	The boot sector in disk is 2 KB (2 KB * (4 pages / 1 KB) = 8 pages)
//		*	ByteFrost memory address space:
//				First 8 KB are ROM		(pages 0 - 31)
//				Middle 48 KB are RAM	(pages 32 - 223)
//				Last 8 KB are MMIO		(pages 224 - 255)
//		->	Load pages 32 - 39 with the boot code
//	2.	Jump to the boot code.

.define	1 NEW_LINE	 	0x10
.define 1 SPACE			0x20
.define 1 _1				0x31
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
.define 1 _v				0x76
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

//	1.	Print "BFSv1 ROM BOOT\n"
OUT _B, A
OUT _F, A
OUT _S, A
OUT _v, A
OUT _1, A
OUT SPACE, A
OUT _R, A
OUT _O, A
OUT _M, A
OUT SPACE, A
OUT _B, A
OUT _O, A
OUT _O, A
OUT _T, A
OUT NEW_LINE, A

//	2.	Copy first 8 pages from disk to pages 32 - 39
.define	2 sdcard 			0xE000
LDA %DP, H, sdcard[1]
LDA %DP, L, sdcard[0]

//	Set SDCard command
//	Byte 0: Page
//	Byte 1: Sector High (3 lsb)
//	Byte 2: Sector Low
//	Byte 3: Go
//	SD Card Command format (16 bit command):
//	15	14	13	12	11	10-0
//	R/W	?	?	?	?	Sector ID

//	R0: i = 0 to 7 (page counter)
//	R1: SDCard command byte (0 - 3)
//	R2: SDCard command byte value

LDR R0, #0		//	for (i = 0; i < 8; i++)
:for_loop 		
MOV R3, R0		//	R3 = i
SUB R3, #8		//	R3 = i - 8 (i < 8 -> i - 8 < 0)
BPL :after_for_loop
LDR R1, #0		//	(Setting page)
LDR R2, #32		//	(Page = 32 + i)
ADD R2, R2, R0
SMR R2, R1		//	Set Page to be 32 + i
INC R1			//	(Setting Sector High)
LDR R2, #0x00	//	Set Sector High = 0x00
SMR R2, R1		
INC R1			//	(Setting Sector Low)
MOV R2, R0		//	Sector Low = i
SMR R2, R1
INC R1			//	GO (R2 is ignored)
SMR R2, R1

INC R0
JMP :for_loop

:after_for_loop		//	Jump to boot code

//	NOTE: Currently this cannot compile. Need to update the assembler
//	to allow for something like
//	:hardcodedLabelAddress = 2000
//	then
//	JMP :hardcodedLabelAddress
.define 2	RAM_START	0x2000
JMP :__RAM_START__
