//	ByteFrost boot code (first mutable code).
//	This assembly file will be compiled and stored as "boot.bin", which will be
//	packed into a ByteFrost disk file (using the BFSv1 filesystem).
//	This boot code is trivial in that it doesn't do anything besides printing
//	"Running first mutable code."

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
.define	1 PERIOD			0x2e

//	Print "BOOT.BIN\n"
OUT _B, A
OUT _O, A
OUT _O, A
OUT _T, A
OUT PERIOD, A
OUT _B, A
OUT _I, A
OUT _N, A
OUT NEW_LINE, A

BRK
