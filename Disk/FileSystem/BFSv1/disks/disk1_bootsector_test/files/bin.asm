//	ByteFrost boot sector code (first mutable code loaded by ROM).
//	Updated: May 15, 2025
//	This assembly file will be compiled and stored as "boot.bin", which will be
//	packed into a ByteFrost disk file (using the BFSv1 filesystem) as the boot
//	sector.
//	This boot code runs a simplified "ls" program that prints the files in the
//	disk's root directory.

.start	0x2000	//	This code will be loaded by the ROM at address 0x2000 (first
				//	RAM address

//	A Simple `ls` Implementation (!!!)
//	This program assumes:
//		1.	The ByteFrost has a connected SD card that has a disk file which is
//			using the ByteFrost Filesystem Version 1 (BFSv1).
//	In the ByteFrost Filesystem Version 1, the ByteFrost disk has the following
//		properties:
//		* block size = sector size = 256 bytes
//		Block Range	# blocks in range	Size		Use
//		0 - 7		8					2 KB		Boot code
//		8			1					256 bytes	Directory block
//		... (rest of disk layout is specified in the ByteFrost FileSystem.pdf
//			file)
//		
//	This program does the following:
//	0.	Print "$ ls\n"
//	1.	Load block 8 (sector 8) that contains the disk root (and only) directory
//		block into RAM at page 30.
//	2.	For each directory entry (0 through 15):
//		*	Each directory entry is 16 bytes long and has the following form:
//			[14 bytes filename][2 bytes inode block pointer (block ID)]
//		Note:
//			*	The filename can be at most 14 characters long. This means:
//				If the length of the filename is < 14 characters, it will be
//				terminated with a \0 byte (0 byte).
//				If the length of the filename == 14 characters, it will not be
//				terminated with a \0 byte.
//				Hence, to read a filename, read from the first byte in the
//				directory entry until either a \0 byte is reached or the max
//				length (14 bytes) has already been reached (and add a \0 byte
//				yourself).
//			*	A directory entry is invalid if the inode block pointer is 0.
//		1.	Read the inode block pointer (last 2 bytes) of the directory entry.
//			If both are 0, skip to the next directory entry.
//		2.	If this is not the first directory entry printed:
//			1.	Print a ' '.
//		3.	Print the filename of this directory entry. 
//			*	Note: This WON'T include the file type extension! To get that,
//				the inode for this file must be read from disk.
//	3.	Print a '\n$ '.
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
.define	1 dollar			0x24
.define 1 _l				0x6c
.define 1 _s				0x73

.define	1 dir_entry_page	0x30
.define	2 sdcard 			0xE000

//	0.	Print "$ ls\n"
OUT dollar, A
OUT SPACE, A
OUT _l, A
OUT _s, A
OUT NEW_LINE, A

//	1.	Load block 8 (sector 8) that contains the disk root (and only) directory
//		block into RAM at page 30.
//	SD Card MMIO addresses (starting at 0xE000):
//  Byte 0: Page
//  Byte 1: Sector High ( 3 lsb)
//  Byte 2: Sector Low
//  Byte 3: Go 
//
//	SD Card Command format (16 bit command) (Bytes 1 and 2):
//	15	14	13	12	11	10-0
//	R/W	?	?	?	?	Sector ID
LSP %HDP, sdcard[1]		//	Set current page to 0xE0

//	Set Byte 0 (Page) to 0x30. 
LDR R0, #0
LDR R1, dir_entry_page
SMR R1, R0		//	*({0xE0, 0x00}) = 0x30

//	Set Byte 1 (Sector High) to 0x00
INC R0
LDR R1, #0x00
SMR R1, R0		//	*({0xE0, 0x01}) = 0x00

//	Set Byte 2 (Sector Low) to 0x08
INC R0
LDR R1, #0x08
SMR R1, R0		//	*({0xE0, 0x02}) = 0x08

//	Write into Byte 3 (Go)
INC R0
SMR R1, R0		//	GO (R1's value is ignored)

//	At this point, the SD card arduino takes over the busses and copies sector 8
//	(block 8)'s contents into page 30.

//	2.	Iterate through the directory entries and print each file's filename.
.define	1	NUM_DIR_ENTRIES			16
.define	1	DIR_ENTRY_SIZE_BYTES	16

//	Pseudocode:
//	byte * currEntry = dir_entry_page (0x3000) + 14 (after file name)
//	for (byte i = 0; i < 16; i++, currEntry += 16) {
//		//	At start of each iteration, currEntry points at the inode block
//		//	pointer of the current directory entry (index 14 out of 15)
//		//	Check that file exists
//		if (*currEntry == 0 && *(currEntry + 1) == 0) {
//			continue;
//		}
//		//	File exists; print file name
//		byte * name = currEntry - 14;
//		while (name != currEntry && *name != '\0') {
//			print *name;
//			name++;
//		}
//		print ' ';
//	}

//	Register Variable Assignment
//	R0:	byte * currEntry (low byte of address)
//	R1: byte i
//	R2: byte * name (low byte of address) or temp if name * is not used
//	R3: temp

//	byte * currEntry = dir_entry_page (0x3000) + 14 (after file name)
LSP %HDP, dir_entry_page
LDR R0, #14					//	R0 = low byte of currEntry
LDR R1, #0					//	R1 = i = 0
:for_loop					//	for (byte i = 0; i < 16; i++, currEntry += 16) {
LDR R2, NUM_DIR_ENTRIES		//	R2 = NUM_DIR_ENTRIES = 16
SUB R3, R1, R2				//	R3 = R1 - R2 = i - 16
BPL :after_for_loop			//	if i - 16 >= 0, skip for loop
//		if (*currEntry == 0 && *(currEntry + 1) == 0) {
LMR R2, R0					//	R2 = *({dir_entry_page, R0}) = *currEntry
SUB R2, #0					//	if *currEntry != 0, skip if statement
BNE :after_if
INC R0						//	R0 = currEntry + 1 (point at byte at index 15)
LMR R2, R0					//	R2 = *(currEntry + 1)
DEC R0						//	R0 = currEntry (restore R0)
SUB R2, #0					//	if *(currEntry + 1) != 0, skip if statement
BNE :after_if
JMP :for_loop_advance		//	file doesn't exist - continue
:after_if					//	file exists - print its name
MOV R2, R0					//	R2 = currEntry
SUB R2, #14					//	R2 = currEntry - 14 = name
:while_loop					//	while (name != currEntry && *name != '\0') {
SUB R3, R2, R0				//	R3 = R2 - R0 = name - currEntry
BEQ :after_while_loop		//	if name == currEntry, skip while loop
LMR R3, R2					//	R3 = *name
SUB R3, #0					//	if *name == '\0', skip while loop
BEQ :after_while_loop
OUT R3, A					//	Print *name
INC R2						//	name++;
JMP :while_loop				//	}
:after_while_loop
OUT SPACE, A				//	Print ' ';
:for_loop_advance
INC R1						//	i++;
ADD R0, #15					//	currEntry += 16;
INC R0
JMP :for_loop
:after_for_loop

//	3.	Print a '\n$ '.
OUT NEW_LINE, A
OUT dollar, A
OUT SPACE, A

BRK
