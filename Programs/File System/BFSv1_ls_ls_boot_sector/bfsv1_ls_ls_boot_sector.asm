//	A Simple ls -ls Implementation (!!!) for BFSv1
//	Assumes:
//		1.	The ByteFrost has a connected SD card that has a disk file using the
//			ByteFrost Filesystem Version 1 (BFSv1).
//	In the ByteFrost Filesystem Version 1, the ByteFrost disk has the following
//	properties:
//	*	block size = sector size = 256 bytes
//	+-------------+-------------------+-----------+-----------------+
//	| Block Range | # blocks in range | Size      | Use             |
//  +-------------+-------------------+-----------+-----------------+
//	| 0 - 7       | 8                 | 2 KB      | Boot code       |
//	| 8           | 1                 | 256 bytes | Directory block |
//	| ...         | ...               | ...       | ...             |
//
//	This program does the following:
//	1.	Print "$ ls -ls\n" (pretends it's a shell)
//	2.	Load block 8 (sector 8) that contains the disk root (and only) directory
//		block into RAM at page 0x30 (48)
//	3.	For each directory entry (0 through 15):
//		*	Each directory entry is 16 bytes long and has the following format:
//			[14 byres filename][2 bytes inode block pointer (block ID)]
//		Note:
//			*	The filename can be at most 14 characters long. Hence:
//				1.	If the length of the filename is < 14 characters, it will be
//					terminated with a \0 sentinel (0 byte).
//				2.	If the length of the filename == 14 characters, it will not
//					be terminated with a \0 sentinel.
//			*	Therefore, to read a filename, read from the first byte in the
//				directory entry until either a \0 sentinel is reached OR the max
//				length (14 bytes) has already been reached (requiring adding a
//				\0 sentinel manually for generic string use).
//			*	A direcory entry is INVALID if the inode block pointer is 0.
//				(invalid meaning unused)
//		1.	Read the inode block pointer (last 2 bytes) of the directory entry.
//			1.	If the inode block pointer is 0, skip to the next directory
//				entry.
//		2.	Print the file entry using ls -ls format, specifying:
//			*	-l (use long format)
//			*	-s (print the file size in disk blocks)
//

//	Pseudocode:
//	printf("$ ls -ls\n");
//	1.	Load block 8 (sector 8) that contains the disk root (and only) directory
//		block into RAM at page 0x30 (48).
//	2.	Set total_blocks (a 16-bit counter since there are 2048 total blocks in
//		the disk) to 0.
//	3.	For each directory entry (0 through 15):
//		1.	If the inode block pointer of the current entry is 0, skip to the
//			next entry.
//		2.	Load the inode block of the current entry at page 0x31 + i (where i
//			is the current directory entry index).
//		3.	Increment total_blocks by the file size field in the inode.
//	4.	printf("total %d\n", total_blocks);
//	5.	For each directory entry (0 through 15):
//		*	[block count (2 chars)][space][size in bytes (4 chars)][space][filename 14 chars]
//			(effectively, filenames must be at most 12 chars to fit on screen in
//			one line)
//		1.	If the inode block pointer of the current entry is 0, skip to the
//			next entry.
//		2.	Read the file size field in blocks of this entry's inode and print
//			it.
//		3.	Print space.
//		4.	Read the file size field in bytes of this entry's inode and print it
//			(requires to GPRs).
//		5.	Print space.
//		6.	Print the filename.
//		7.	Print newline.

.start 0x2000

//	Offsets assuming base address is at 0xE000
.define	1 DISK_MMIO_PAGE_OFFSET			0
.define	1 DISK_MMIO_SECTOR_HI_OFFSET	1
.define 1 DISK_MMIO_SECTOR_LO_OFFSET	2
.define 1 DISK_MMIO_GO_OFFSET			3

.define	1 NUM_DIR_ENTRIES		16
.define	1 DIR_ENTRY_SIZE_BYTES	16

.define	1 DirectoryEntry_filename_offset	0
.define 1 DirectoryEntry_inode_block_sector_offset_lo	15
.define 1 DirectoryEntry_inode_block_sector_offset_hi	14

//  Memory use:
//  Page 0x30: Directory block
//  Page 0x31 - 0x46: Inode array
//      *   Directory entry index in directory block is used as index to
//          this array.
//  Stack pointer at 0xDFFF.
//  Stack breakdown:
//  SP:		total_blocks (high)
//	SP - 1:	total_blocks (low)
.define	1 dir_block_page		0x30
.define 1 inode_arr_base_page	0x31
.define	2 sdcard 				0xE000
.define	1 sp_total_blocks_hi	0
.define	1 sp_total_blocks_lo	-1

////////////////////////////////////////////////////
//  Implementation

//	0.	Set SP to 0xDFFF.
LDA %SP, H, #0xDF
LDA %SP, L, #0xFF

//	1.	printf("$ ls -ls\n");
OUT '$'
OUT ' '
OUT 'l'
OUT 's'
OUT ' '
OUT '-'
OUT 'l'
OUT 's'
OUT '\n'

//	2.	Load block 8 that contains the disk root (and only) directory block into
//		RAM at page 0x30.

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
LDA %DP, H, sdcard[1]
LDA %DP, L, sdcard[0]

//	Set Byte 0 (Page) to 0x30
LDR R0, dir_block_page
SDW R0, %DP, DISK_MMIO_PAGE_OFFSET

//	Set Byte 1 (Sector High) to 0x00
LDR R0, #0x00
SDW R0, %DP, DISK_MMIO_SECTOR_HI_OFFSET

//	Set Byte 2 (Sector Low) to 0x08
LDR R0, #0x08
SDW R0, %DP, DISK_MMIO_SECTOR_LO_OFFSET

//	Write into Byte 3 (Go) (R0's value is ignored)
SDW R0, %DP, DISK_MMIO_GO_OFFSET

//	At this point, the disk interface takes over the busses and copies sector 8
//	(block 8)'s contents into page 0x30.

//	3.	Set total_blocks (a 16-bit counter as there are 2048 total blocks in the
//		disk) to 0.
LDR R0, #0x00
SDW R0, %SP, sp_total_blocks_hi
SDW R0, %SP, sp_total_blocks_lo

//	4.	For each directory entry (0 through 15):
//		R0: index of directory entry (i)
//		R1: Inode page (inode_arr_base_page + i)
//		DP: dir_block_page (0x3000) (points at current directory entry)
LDR R0, #0						//	i = 0;
LDR R1, inode_arr_base_page		//	inode_page = 0x31;
LDA %DP, H, dir_block_page		//	DirectoryEntry * entry = 0x3000;
LDA %DP, L, #0

:loop_1					//	for (; i < 16;) {

LDR R3, #16
TST R0, R3				//		R0 - 16 = i - 16 (since i < 16 -> i - 16 < 0)
BPL :loop_1_done		//		(Since if i < 16, i - 16 < 0 and negative flag is set)

//		1.	If the inode block pointer of the current entry is 0, skip to the
//			next entry.
//							if (entry->inode_block_sector == 0) continue;
LDW R2, %DP, DirectoryEntry_inode_block_sector_offset_lo
TST R2, #0
BNE :handle_dir_entry
LDW R2, %DP, DirectoryEntry_inode_block_sector_offset_hi
TST R2, #0
BNE :handle_dir_entry
JMP :loop_1_update

:handle_dir_entry
//		2.	Load the inode block of the current entry at page 0x31 + i.
//			(Use BP as temp address register).
LDA %BP, H, sdcard[1]
LDA %BP, L, sdcard[0]

//	Set Byte 0 (Page) to 0x31 + i (R1)
SDW R1, %BP, DISK_MMIO_PAGE_OFFSET

//	Set Byte 1 (Sector High) to entry->inode_block_sector[1]
LDW R2, %DP, DirectoryEntry_inode_block_sector_offset_hi
SDW R2, %BP, DISK_MMIO_SECTOR_HI_OFFSET

//	Set Byte 2 (Sector Low) to entry->inode_block_sector[0]
LDW R2, %DP, DirectoryEntry_inode_block_sector_offset_lo
SDW R2, %BP, DISK_MMIO_SECTOR_LO_OFFSET

//	Write into Byte 3 (Go) (R0's value is ignored)
SDW R0, %BP, DISK_MMIO_GO_OFFSET

//		3.	Increment total_blocks by the file size field (in blocks) in that
//			inode.
//			(Use BP as temp address register pointing at the inode in RAM).

//	Set BP to (inode_page << 8)
MGA %BP, H, R1
LDA %BP, L, #0

//	R2 = entry_inode->file_size_blocks. (offset: 2 bytes)
LDW R2, %BP, #2

//	Now need to read total_blocks; store total_blocks low in R3.
LDW R3, %SP, sp_total_blocks_lo

//	Add total_blocks low with file_size_blocks (R2)
ADD R2, R2, R3

//	Load total_blocks high in R3.
LDW R3, %SP, sp_total_blocks_hi

//	Add total_blocks_hi with 0 and carry.
ADC R3, #0

//	Write total_blocks to stack.
SDW R3, %SP, sp_total_blocks_hi
SDW R2, %SP, sp_total_blocks_lo

:loop_1_update
//	Update variables:
//		R0: index of directory entry (i)
//		R1: Inode page (inode_arr_base_page + i)
//		DP: dir_block_page (0x3000) (points at current directory entry)
//	1.	R0 += 1
//	2.	R1 += 1
//	3.	DP += sizeof(DirectoryEntry) = 16
INC R0
INC R1
MAA %DP, %DP, DIR_ENTRY_SIZE_BYTES
JMP :loop_1

:loop_1_done

//	5.	printf("total %hu\n", total_blocks);
OUT 't'
OUT 'o'
OUT 't'
OUT 'a'
OUT 'l'
OUT ' '
LDW R2, %SP, sp_total_blocks_hi
//	Print high byte if not zero.
TST R2, #0
BEQ :print_low_total_blocks
OUT R2, I
:print_low_total_blocks
LDW R2, %SP, sp_total_blocks_lo
OUT R2, I

OUT '\n'

//	6.	For each directory entry (0 through 15):
//		R0: index of directory entry (i)
//		R1: Inode page (inode_arr_base_page + i)
//		DP: dir_block_page (0x3000) (points at current directory entry)
//		BP: Inode entry (0x3100)
LDR R0, #0						//	i = 0;
LDR R1, inode_arr_base_page		//	inode_page = 0x31;
LDA %DP, H, dir_block_page		//	DirectoryEntry * entry = 0x3000;
LDA %DP, L, #0
LDA %BP, H, inode_arr_base_page	//	Inode * entry_inode = 0x3100;
LDA %BP, L, #0

:loop_2					//	for (; i < 16;) {
//OUT ASTERISK, A
LDR R3, #16
TST R0, R3 				//		R0 - 16 = i - 16 (since i < 16 -> i - 16 < 0)
BPL :loop_2_done		//		(Since if i < 16, i - 16 < 0 and negative flag is set)

//		1.	If the inode block pointer of the current entry is 0, skip to the
//			next entry.
//							if (entry->inode_block_sector == 0) continue;
LDW R2, %DP, DirectoryEntry_inode_block_sector_offset_lo
TST R2, #0
BNE :handle_dir_entry_2
LDW R2, %DP, DirectoryEntry_inode_block_sector_offset_hi
TST R2, #0
BNE :handle_dir_entry_2
JMP :loop_2_update

:handle_dir_entry_2

//		2.	Read the file size field in blocks of this entry's inode and print
//			it.

//	Read entry_node->file_size_blocks
LDW R2, %BP, #2
OUT R2, I

//		3.	Print space.
OUT ' '

//		4.	Read the file size field in bytes of this entry's inode

//	Read entry_node->file_size_bytes HIGH byte
LDW R2, %BP, #0
OUT R2, I

//	Read entry_node->file_size_bytes LOW byte
LDW R2, %BP, #1
OUT R2, I

//		5.	Print space.
OUT ' '

//		6.	Print file type

//	Read entry_node->file_type
LDW R2, %BP, #3

//	Map type value to a character to print
//	1.	0 --> UNKNOWN
//	1.	1 --> TEXT
//	2.	2 --> ASM
//	3.	3 --> MLG
//	4.	4 --> BIN
TST R2, #0
BNE :file_type_text_check
OUT 'U'
JMP :file_type_check_done

:file_type_text_check
TST R2, #1
BNE :file_type_asm_check
OUT 'T'
JMP :file_type_check_done

:file_type_asm_check
TST R2, #2
BNE :file_type_mlg_check
OUT 'A'
JMP :file_type_check_done

:file_type_mlg_check
TST R2, #3
BNE :file_type_bin_check
OUT 'M'
JMP :file_type_check_done

:file_type_bin_check
OUT 'B'

:file_type_check_done

//		7.	Print space.
OUT ' '

//		8.	Print the filename.
//			1.	for (R2 = 0; R2 < 14;):
//				1.	R3 = *DP.
//				2.	if R3 == '\0', break
//				3.	Print R3
//				4.	R2++, DP++
//			2.	Move DP to next directory entry:
//				1.	for (R2 < 16):
//					1.	DP++, R2++
LDR R2, #0
:file_name_print_loop
LDR R3, #14
TST R2, R3		//	R2 < 14 -> R2 - 14 < 0
BPL :file_name_print_loop_done
LDW R3, %DP, #0
TST R3, #0
BEQ :file_name_print_loop_done
OUT R3, A

INC R2
MAA %DP, %DP, #1
JMP :file_name_print_loop

:file_name_print_loop_done

:move_dp_to_next_entry_loop
// OUT R2, I
 
LDR R3, #16
TST R2, R3
BPL :done_entry
INC R2
MAA %DP, %DP, #1
JMP :move_dp_to_next_entry_loop

:done_entry

//		9.	Print newline.
OUT '\n'

:loop_2_update

INC R0
INC R1
MGA %BP, H, R1
JMP :loop_2

:loop_2_done

//	7.	Print "$ "
OUT '$'
OUT ' '

BRK