.define 8 PI -0xdeadbeef
.define 2 PI2 0x2000
//.banana
ADD R1, R2, R3	//	this is a comment
//ADD R1, :label[2]
	NOP
				//	This comment includes an invalid token: 1_2
				//This is not a comment and includes an invalid token:
//		     :label_test	  // another comment
:label
MOV R1, R2
LDR R1, #-128
LDR R1, PI[0]
LDR R1, :label[0]
:label2
JMP :label
JMP :label2
OR R2, #14
OUT R3, A
CALL :label2
BRK
