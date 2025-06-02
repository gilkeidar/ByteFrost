//	True Random Number Generation MMIO Device Test
//	This code is stored in the boot sector of a ByteFrost
//	disk file

.start	0x2000

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

//	Generate at least 1 million bits of random numbers
//	--> (1 million bits) * (1 byte / 8 bits) * (1 sector / 256 bytes)
//		= 488.28125 sectors --> 489 sectors, or alternatively, just generate
//		512 sectors' worth.
//	Goal: Write random numbers to 512 sectors in disk.
//	1.	for i = 0 to 1:
//		*	Write to 256 sectors.
//		1.	currSector = 27.
//		2.	if i == 1: currSector += 256.
//		3.	for j = 0 to 255:
//			*	Write to currSector.
//			1.	for k = 0 to 255:
//				*	Write one random byte to byte k of page in RAM.
//				1.	randomByte = rand().
//				2.	*(0x30, k) = randomByte.
//			2.	Write page 30 to currSector.
//			3.	currSector++.

//	1.	for i = 0 to 1:
//		1.	Set Sector High to 128 | (1 + i). (128 to specify that command is 
//			to write to the disk)
//		2.	for j = 0 to 255:
//			1.	Set Sector Low to j.
//			2.	for k = 0 to 255:
//				1.	tmp = rand().
//				2.	*(0x30, k) = tmp.
//			3.	Send Go command. (write to current sector).

//	Register Variable Assignment
//	R0: Sector High
//	R1:	byte j
//	R2:	byte k
//	R3: tmp

.define	1	tmp_page	0x30
.define 2	sdcard		0xE000
.define 2	trng		0xE200

LSP %HDP, sdcard[1]
LDR R3, #0x30
SMA R3, #0x00		//	Set Page to 0x30.
LDR R0, #129		//	R0 = 128 | 1 = 129. (i = 0).
:for_i_loop
LDR R3, #131		//	R3 = 131
SUB R3, R3, R0		//	R3 = R3 - R0 = 131 - R0
BEQ :end_loop_i		//	If 131 - R0 == 0, exit for loop.
LDR R1, #0			//	j (R1) = 0
:do_for_j_loop		//	(need to loop at least once to check j overflow)
LDR R2, #0			//	k (R2) = 0
:do_for_k_loop
LSP %HDP, trng[1]	//	Set current page to 0xE2 (cycle counter / trng).
LMA R3, #0x06		//	R3 = rand() (*(0xE206)).
LSP %HDP tmp_page	//	Set current page to 0x30.
SMR R3, R2			//	*({0x30, k}) = R3.
INC R2				//	k++ (R2++)
BNE :do_for_k_loop	//	If k is not 0 (i.e., didn't overflow), do loop again.
LSP %HDP, sdcard[1]	//	Set current page to 0xE0.
SMA R0, #0x01		//	*(0xE001) = R0 (set Sector High)
SMA R1, #0x02		//	*(0xE002) = j (R1) (set Sector Low to j)
SMA R0, #0x03		//	Go (R0's value is ignored).
INC R1				//	j++
BNE :do_for_j_loop	//	If j is not 0 (i.e., didn't overflow), do loop again.
INC R0				//	R0++
JMP :for_i_loop
:end_loop_i			//	Print "DONE"
OUT _D, A
OUT _O, A
OUT _N, A
OUT _E, A
BRK














//	Original implementation (generate random numbers
//	in an infinite loop and print to the screen)
//	
//	while (true) {
//		R1 = rand();
//		print R1;
//	}

// .start	0x2000	//	Loaded into RAM
// .define	2	device1		0xE200
// .define 1 SPACE			0x20

// LSP %HDP, device1[1]
// :loop
// LMA R1, #0x06
// OUT R1, I
// OUT SPACE, A
// NOP
// NOP
// NOP
// NOP
// JMP :loop