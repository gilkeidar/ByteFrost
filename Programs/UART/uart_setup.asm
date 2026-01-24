
.define	1 NEW_LINE	 		0x10
.define 1 SPACE				0x20
.define 1 LEFT_PAR			0x28
.define	1 RIGHT_PAR			0x29
.define 1 COMMA				0x2c
.define 1 ASTERISK			0x2a
.define 1 DASH				0x2d
.define 1 DOT				0x2e
.define 1 COLON				0x3a
.define 1 EQUALS			0x3d

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

.define 1 _l				0x6c
.define 1 _m				0x6d
.define 1 _n				0x6e
.define 1 _o				0x6f
.define 1 _p				0x70
.define 1 _q				0x71
.define 1 _r				0x72
.define 1 _s				0x73
.define 1 _t				0x74
.define 1 _u				0x75
.define 1 _v				0x76
.define 1 _w				0x77
.define 1 _x				0x78
.define 1 _y				0x79
.define 1 _z				0x7a

OUT NEW_LINE, A   // Space from previous run on the minicom
OUT NEW_LINE, A
OUT _U, A
OUT _A, A
OUT _R, A
OUT _T, A
OUT SPACE, A
OUT _S, A
OUT _E, A
OUT _T, A
OUT _U, A
OUT _P, A
OUT NEW_LINE, A

// Base Address

.define	2 UART			0xE400
LDA %DP, H, UART[1]
LDA %DP, L, UART[0]

// Registers:
//	Address		R/W	Name		Description
//  -------     --- ----		-----------
//	0xE400		R/W	MR1, MR2	Mode Registers. Parity, bits per char, stop bit, etc
.define 1 MR1_MR2_RW 0
//	0xE401		R	SR			Status Register
.define 1 SR_R 		1			 
//	0xE401		W	CSR			Clock Select for Rx and Tx
.define 1 CSR_W		1			 
//	0xE402		R	BRG Test
.define 1 BRG_Test_R 2	 
//	0xE402		W	CR
.define 1 CR_W 		2
//	0xE403		R	RHR
.define 1 RHR_R 	3			 
//	0xE403		W	THR
.define 1 THR_W 	3			 
//	0xE404		R	1X/16X Tst
.define 1 X_16X_R 	4
//	0xE404		W	ACR
.define 1 ACR_W 	4
//	0xE405		R	ISR			 
.define 1 ISR_R 	5
//	0xE405		W	IMR
.define 1 IMR_W 	5
//	0xE406		R	CTU			 
.define 1 CTU_R 	6
//	0xE406		W	CTUR
.define 1 CTUR_W 	6
//	0xE407		R	CTL			 
.define 1 CTL_R 	7
//	0xE407		W	CTLR
.define 1 CTLR_W 	7

:init_uart
// Reset
LDR R1, #0x2A 	// Reset Rx, Disable Rx, Tx
SDW R1, %DP, CR_W
LDR R1, #0x35 	// Reset Tx, Enable Rx, Tx
SDW R1, %DP, CR_W

// ACR Set MPO to show the clock
LDR R1, #0x39 	// Set 1, Counter from X1, Power Down OFF
SDW R1, %DP, ACR_W

// CSR 
LDR R1, #0x99 	// Set Boud Rate to 4800
SDW R1, %DP, CSR_W

// Start Counter
LDR R1, #0x80 	
SDW R1, %DP, CR_W

// Select MR1
LDR R1, #0x10 	// Select MR1
SDW R1, %DP, CR_W

// Check out MR1, MR2
LDW R1, %DP, MR1_MR2_RW
OUT _M, A
OUT _R, A
OUT #1, A
OUT R1, I
OUT SPACE, A
LDW R1, %DP, MR1_MR2_RW
OUT _M, A
OUT _R, A
OUT #2, A
OUT R1, I
OUT NEW_LINE, A

// Select MR1 - Program MR1, MR2
LDR R1, #0x10 	// Select MR1
SDW R1, %DP, CR_W

LDR R1, #0x13 	// MR1: No Parity, 8 bit charecter
SDW R1, %DP, MR1_MR2_RW
LDR R1, #0x07 	// MR2: Normal, 1 stop bit
SDW R1, %DP, MR1_MR2_RW

// Select MR1
 // Verify MR1, MR2
LDW R1, %DP, MR1_MR2_RW
OUT _M, A
OUT _R, A
OUT #1, A
OUT R1, I
OUT SPACE, A
LDW R1, %DP, MR1_MR2_RW
OUT _M, A
OUT _R, A
OUT #2, A
OUT R1, I
OUT NEW_LINE, A

LDW R1, %DP, SR_R
OUT _S, A
OUT _R, A
OUT SPACE, A
OUT R1, I
OUT NEW_LINE, A

LDW R1, %DP, BRG_Test_R
OUT _B, A
OUT _R, A
OUT _G, A
OUT R1, I
OUT NEW_LINE, A

LDW R1, %DP, RHR_R
OUT _R, A
OUT _H, A
OUT _R, A
OUT R1, I
OUT NEW_LINE, A

LDW R1, %DP, X_16X_R
OUT #1, A
OUT #6, A
OUT _X, A
OUT R1, I
OUT NEW_LINE, A

LDW R1, %DP, ISR_R
OUT _I, A
OUT _S, A
OUT _R, A
OUT R1, I
OUT NEW_LINE, A

LDW R1, %DP, CTU_R
OUT _C, A
OUT _T, A
OUT _U, A
OUT R1, I
OUT NEW_LINE, A

LDW R1, %DP, CTL_R
OUT _C, A
OUT _T, A
OUT _L, A
OUT R1, I
OUT NEW_LINE, A


BRK