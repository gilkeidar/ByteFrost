//.define PI 3
ADD R1, R2, R3	//	this is a comment
//ADD R1, :label
	NOP
				//	This comment includes an invalid token: 1_2
				//This is not a comment and includes an invalid token:
//		     :label_test	  // another comment
MOV R1, R2
LDR R1, #5
BRK
