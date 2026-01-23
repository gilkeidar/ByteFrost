//	Test LDW implementation. Runs on bare metal (not from disk).

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

//	0.	(Print test info).
//	1.	Push a value on the stack.
//	2.	Load the written value from the stack using LDW.
//	3.	Check that values are the same.

//	1.	Print "LDW STACK TEST\n"
OUT _L, A
OUT _D, A
OUT _W, A
OUT SPACE, A
OUT _S, A
OUT _T, A
OUT _A, A
OUT _C, A
OUT _K, A
OUT SPACE, A
OUT _T, A
OUT _E, A
OUT _S, A
OUT _T, A
OUT NEW_LINE, A

//	2.	Load R0 with a value and print that value.
LDR R0, #0x25
OUT _R, A
OUT #0, A
OUT COLON, A
OUT SPACE, A
OUT #0, A
OUT _x, A
OUT R0, I
OUT SPACE, A

//	3.	Push R0 to the stack.
PUSH R0

//	4.	Use LDW to read the previous value from the stack into R1.
LDW R1, %SP, #-1


//	5.	Print R1's value and check that it's the same as R0.
OUT _R, A
OUT #1, A
OUT COLON, A
OUT SPACE, A
OUT #0, A
OUT _x, A
OUT R1, I
OUT NEW_LINE, A


//	6.	Done.
BRK
