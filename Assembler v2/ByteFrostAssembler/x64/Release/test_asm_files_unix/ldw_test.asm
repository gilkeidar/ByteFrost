//	LDW Test
LDW R2, %PC, #5
LDW R3, %DP, #-128
LDW R1, %SP, #-5
LDW R0, %BP, #127
//	LDW R2, %BP, #234	//	Should fail! (It failed!)
