//////////////////////////////////
// BISTv3:
// Add Address Registers
//////////////////////////////////
//////////////////////////////////
// BISTv2:
// Enhance to 16bit address range
// 0x0000 .. 0x1FFF  ROM 
// 0x2000 .. 0xDFFF  RAM
// 0xE000 .. 0xFFFF  Registers (MMIO)
// Set Data Pointer to  0x4000
// Set Stack Pointer to 0x7700
//////////////////////////////////

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
.define 1 DOT				0x2e

.define 1 _p				0x70
.define 1 _q				0x71
.define 1 _r				0x72
.define 1 _s				0x73
.define 1 _t				0x74
.define 1 _u				0x75
.define 1 _v				0x76
.define 1 _w				0x77
.define 1 _x				0x78

.define 1 EQUALS			0x3d

OUT _B, A
OUT _I, A
OUT _S, A
OUT _T, A
OUT SPACE, A
OUT _v, A
OUT #3, A
OUT DOT, A
OUT #0, A
OUT NEW_LINE, A
////////////////////////////////////
OUT _O, A
OUT _p, A
OUT #0, I
OUT COLON, A



OUT NEW_LINE, A
////////////////////////////////////
BRK
