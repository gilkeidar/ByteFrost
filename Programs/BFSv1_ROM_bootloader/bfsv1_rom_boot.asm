//	BFSv1 ROM Boot Code
//	Assumes:
//		1.	A BFSv1 filesystem is in use and the ByteFrost disk file used on the
//			disk has its boot sector correctly set up with first mutablle code.

//	This BFSv1 ROM Boot Code does the following:
//	1.	Load the boot sector from disk into RAM (at 0x2000)
//		*	The boot sector in disk is 2 KB (2 KB * (4 pages / 1 KB) = 8 pages)
//		*	ByteFrost memory address space is as follows (64KB, 16-bit address):
//			+-------------------------------+
//			| MMIO (8 KB) (0xE000 - 0xFFFF) |	Pages 224 - 255
//			| (disk, cycle counter, RNG)    |
//			+-------------------------------+
//			| RAM (48 KB) (0x2000 - 0xDFFF) |	Pages 32 - 223
//			|                               |
//			|                               |
//			+-------------------------------+
//			| ROM (8 KB)  (0x0000 - 0x1FFF) |	Pages 0 - 31
//			| (this boot code)              |
//			+-------------------------------+
//		-> Load pages 32 - 39 with the boot code.
//	2.	Jump to the boot code.

//	Character constant definitions
//	NOTE: This is ugly, but the assembler does not yet support character-type
//		  immediates.
.define	1 NEW_LINE	 		0x10
.define 1 SPACE				0x20
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

//	Address constant definitions

//	Disk MMIO base address (for sector read / write interface)
.define 2 sdcard			0xE000

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

//	2.	Copy first 8 pages from disk to pages 32 - 39 in RAM.

//	Set DP to point at the disk MMIO base address.
LDA %DP, H, sdcard[1]
LDA %DP, L, sdcard[0]

//	Disk MMIO interface is as follows:
//	Byte 0 (0xE000): Page (in ByteFrost address space)
//	Byte 1 (0xE001): Command and Sector High (3 lsbs)
//	Byte 2 (0xE002): Sector Low (8 bits)
//	Byte 3 (0xE003): Go (R/W to this address initiates disk operation)
//
//	Bytes 1 and 2 of the interface can be viewed as follows:
//	[Byte 1                           ][Byte 2]
//	15  14  13  12  11  10-8           7-0
//	R/W ?   ?   ?   ?   Sector ID High Sector ID Low
//
//	Note: This means that a Sector ID is 11-bits; given that each sector is the
//	same size as a ByteFrost page (256 bytes), this allows for a 512 KB disk.
//	Note that bits 11 - 14 aren't used, however - without hardware modification,
//	we can therefore expand the disk to 8 MB.
//
//	This allows for a simple read / write sector interface with the disk. The
//	actual copying of the sector to a page or vice-versa is done by the disk
//	interface and is a blocking operation (the ByteFrost CPU is stopped by the
//	disk interface! It stops fetching instructions until the disk operation is
//	complete).

//	R0: i = 0 to 7 (page counter)
//	R1: Disk command byte value
.define	1 DISK_MMIO_PAGE_OFFSET			0
.define	1 DISK_MMIO_SECTOR_HI_OFFSET	1
.define 1 DISK_MMIO_SECTOR_LO_OFFSET	2
.define 1 DISK_MMIO_GO_OFFSET			3

LDR R0, #0			//	for (i = 0; i < 8; i++)
:for_loop			//	{
	LDR R1, #8		//		
	TST R0, R1		//		Perform i - 8 (i < 8 -> i - 8 < 0) to set ALU flags.
					//		(If i < 8, i - 8 < 0 and N flag is set)
	BPL :after_for_loop		
					//		page (R1) = 32 + i
	LDR R1, #32		//			(R1 = 32)
	ADD R1, R1, R0	//			(R1 = 32 + i)
					//		Set MMIO page interface to page (32 + i).
	SDW R1, %DP, DISK_MMIO_PAGE_OFFSET
					//		Set Sector High to 0x00 (Read and top 3 sector ID
					//		bits are 0).
	LDR R1, #0x00
	SDW R1, %DP, DISK_MMIO_SECTOR_HI_OFFSET
					//		Set Sector Low to i.
	SDW R0, %DP, DISK_MMIO_SECTOR_LO_OFFSET
					//		Go (R1 is ignored). ByteFrost blocks here.
	SDW R1, %DP, DISK_MMIO_GO_OFFSET
	
	INC R0			//		(i++);
	JMP :for_loop	//	}
	
:after_for_loop			//	Jump to boot sector code.
	JMP :__RAM_START__	//	Jump to RAM start (0x2000) - page 32.