//////////////////////////////////
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




LSP %HDP, #0x40
LSP %HSP, #0x77

//////////////////////////////////
// Test 1: Shift Left (8: ASL)  //
//////////////////////////////////

OUT #1, I   // Print 01                  
OUT #0x20, A // Print ' '  
OUT #0x41, A // Print 'A'    
OUT #0x53, A // Print 'S'
OUT #0x4C, A // Print 'L'    
OUT #0x3A, A // Print ':'   
     
LDR R2, #0x5A
ASL R2, R2 
OUT R2, I     //            (0xB4)
OUT #0x20, A  // Print ' '    

ASL R2, R2 
OUT R2, I     //            (0x68) + Carry
OUT #0x20, A  // Print ' '    
ASL R2, R2    // (Unlike ROL, ASL does not take the carry in back)
OUT R2, I     //            (0xD0)   
LDR R3, #0xD0 
SUB R3, R3, R2	// (carry is set)   
BEQ :test1_pass // RELATIVE JUMP
BRK           	// Test 1 failed
:test1_pass
ADD R3, #0x01 // (reset carry)
OUT #0x10, A     // Newline

//////////////////////////////////
// Test 2: Rotate Left (9: ROL) //
//////////////////////////////////
OUT #2, I   // Print 02                  
OUT #0x20, A // Print ' '  
OUT #0x52, A // Print 'R'    
OUT #0x4F, A // Print 'O'
OUT #0x4C, A // Print 'L'    
OUT #0x3A, A // Print ':'   

LDR R2, #0x34
ROL R2, R2    // 34 -> 68 
OUT R2, I     //           (0x68)
OUT #0x20, A  // Print ' '  
ROL R2, R2    // 68 -> D0 
OUT R2, I     //           (0xD0)
OUT #0x20, A  // Print ' '  
ROL R2, R2    // D0 -> A0 + C 
OUT R2, I     //           (0xA0)
OUT #0x20, A  // Print ' '  
ROL R2, R2    // C+A0 -> 41 + C 
OUT R2, I     //           (0x41)
BCS :test2_check1      // RELATIVE JUMP
BRK           // Test 2 failed: Carry was not set
:test2_check1
LDR R3, #0x41 
SUB R3, R3, R2
BEQ :test2_pass         // RELATIVE JUMP
BRK           // Test 2 failed: Result is not 0x41  
:test2_pass  
OUT #0x10, A  // Newline
    
///////////////////////////////////////////
// Test 3: Logic Shift Right  (A: LSR)   //
///////////////////////////////////////////
OUT #3, I   // Print 03                  
OUT #0x20, A // Print ' '  
OUT #0x4C, A // Print 'L'  
OUT #0x53, A // Print 'S'
OUT #0x52, A // Print 'R'    
OUT #0x3A, A // Print ':'   
  
LDR R2, #0x87
LSR R2, R2    // 87 -> 43 + Carry
OUT R2, I     //           (0x43)
OUT #0x20, A  // Print ' ' 
LSR R2, R2    // 43 -> 21 + Carry
OUT R2, I     //           (0x21)
OUT #0x20, A  // Print ' ' 
BCS :test3_check1       // Relative
BRK           // Test 3 failed: Carry was not set
:test3_check1
LDR R3, #0x21 
SUB R3, R3, R2
BEQ :test3_pass       // relative
BRK           // Test 3 failed: Result is not 0x21 
:test3_pass  
OUT #0x10, A  // Newline

//////////////////////////////////////////////
// Test 4: Arithmetic Shift Right  (B: ASR) //
//////////////////////////////////////////////
OUT #4, I   // Print 04                  
OUT #0x20, A // Print ' '  
OUT #0x41, A // Print 'A'   
OUT #0x53, A // Print 'S'
OUT #0x52, A // Print 'R'    
OUT #0x3A, A // Print ':'   

LDR R2, #0x87
ASR R2, R2    // 87 -> C3 + Carry
OUT R2, I     //           (0xC3)
OUT #0x20, A  // Print ' '  
ASR R2, R2    // C3 -> E1 + Carry
OUT R2, I     //           (0xE1)
BCS :test4_check1       // Relative
BRK           // Test 4 failed: Carry was not set
:test4_check1
LDR R3, #0xE1 
SUB R3, R3, R2
BEQ :test4_pass       // Relative
BRK           // Test 4 failed: Result is not 0xE1
:test4_pass    
OUT #0x10, A  // Newline

//////////////////////////////////////////////
// Test 5: Add 0x1234 + 0x6DCD =  0x8001    //
//////////////////////////////////////////////
 
    OUT #5, I   // Print 05                  
    OUT #0x20, A // Print ' '  
    OUT #0x41, A // Print 'A'   
    OUT #0x44, A // Print 'D'
    OUT #0x44, A // Print 'D'    
    OUT #0x3A, A // Print ':'   

    LDR R2, #0x34
    LDR R1, #0xCD
    ADD R3, R2, R1    // 34 + CD =  0x01 + Carry
    OUT R3, I    //  (0x01)
    OUT #0x20, A // Print ' '  
    
    BCS :test5_check1      // Relative
    BRK          // Test 5 failed: Carry was not set
	:test5_check1
    LDR R2, #0x12
    LDR R1, #0x6D
    ADC R3, R2, R1  // 0x80 = 0x12 + 0x6D + Carry  
    OUT R3, I    //       (0x80)
    BCC :test5_check2      // Relative
    BRK          // Test 5 failed: Carry was  set
	:test5_check2
    LDR R2, #0x80
    SUB R3, R3, R2
    BEQ :test5_pass      // Relative
    BRK          // Test 5 failed: Result is not 0x80    
	:test5_pass
    OUT #0x10, A  // Newline

//////////////////////////////////////////////
// Test 6: Sub (0xA765 – 0x34AB) = 0x72BA   //  
//////////////////////////////////////////////

    OUT #6, I   // Print 06                  
    OUT #0x20, A // Print ' '  
    OUT #0x53, A // Print 'S'
    OUT #0x55, A // Print 'U'
    OUT #0x42, A // Print 'B'    
    OUT #0x3A, A // Print ':'   

    LDR R2, #0x65
    LDR R1, #0xAB
    SUB R3, R2, R1    // 65 - AB =  0xBA + Carry
    OUT R3, I         // (0xBA) (No carry, means borrow)
    OUT #0x20, A      // Print ' '  
    BCC :test6_check1           // Relative
    BRK               // Test 6 failed: Carry was set
	:test6_check1
    LDR R2, #0xA7
    LDR R1, #0x34
    SBC R3, R2, R1    // R2 - R1 + Cin = A7 - 34 + Cin = 72 + Cout
    OUT R3, I         //  (0x72)
    BCS :test6_check2           // Relative
    BRK               // Test 6 failed: Carry was not set
	:test6_check2
    LDR R2, #0x72
    SUB R3, R3, R2
    BEQ :test6_pass      // Relative
    BRK          // Test 6 failed: Result is not 0x72    
	:test6_pass
    OUT #0x10, A  // Newline

////////////////////////////////////
// Test 7: Rotate Right (C: ROR)  //
////////////////////////////////////

    OUT #7, A   // Print 7                  
    OUT #0x20, A // Print ' '  
    OUT #0x52, A // Print 'R'    
    OUT #0x4F, A // Print 'O'
    OUT #0x52, A // Print 'R'    
    OUT #0x3A, A // Print ':'   

    LDR R2, #0x36
    ADD R2, #0 // Reset Carry
    ROR R2, R2 //    36 -> 1B 
    OUT R2, I  //    (0x1B)
    OUT #0x20, A // Print ' ' 
    ROR R2, R2   // 1B -> 0D + Carry 
    OUT R2, I    // (0x0D)
    OUT #0x20, A // Print ' ' 
    ROR R2, R2   // 0D -> 86 + Carry 
    OUT R2, I    // (0x86)
    OUT #0x20, A // Print ' ' 
    ROR R2, R2   // 86 -> C3 
    OUT R2, I    // (0xC3)
    OUT #0x20, A // Print ' ' 
    ROR R2, R2   // C3 -> 61 + Carry 
    OUT R2, I    // (0x61) 
    BCS :test7_check1      // RELATIVE JUMP
    BRK          // Test 7 failed: Carry was not set
	:test7_check1
    LDR R3, #0x61 
    SUB R3, R3, R2
    BEQ :test7_pass      // RELATIVE JUMP
    BRK          // Test 7 failed: Result is not 0x61   
	:test7_pass
    OUT #0x10, A  // Newline

/////////////////////////////////////////////////////
// Test 8: SRAM  Immediate Load/Store   LMA / SMA  //
/////////////////////////////////////////////////////

    OUT #8, I   // Print 08                  
    OUT #0x20, A // Print ' '  
    OUT #0x78, A // Print 'x'
    OUT #0x4D, A // Print 'M'
    OUT #0x41, A // Print 'A'    
    OUT #0x3A, A // Print ':'   

    LDR R1, #0x11
    SMA R1, #0x10 // Store 11 in address 10
    LDR R2, #0x22
    SMA R2, #0x20 // Store 22 in address 20
    LDR R3, #0x33
    SMA R3, #0x30 // Store 33 in address 30
  
    LMA R3, #0x20 // Load value at address 20 into R3 (R3 = 22)
    LMA R2, #0x10 // Load value at address 10 into R2 (R2 = 11)
    LMA R1, #0x30 // Load value at address 30 into R1 (R1 = 33)
    OUT R1, I     // (0x33)
    OUT #0x20, A  // Print ' ' 
    SUB R1, R1, R2
    OUT R1, I     // (0x22)
    OUT #0x20, A  // Print ' ' 
    SUB R1, R1, R3
    OUT R1, I     // (0x00)
    BEQ :test8_pass       // RELATIVE JUMP
    BRK           // Test 8 failed: Result is not 0x00    
	:test8_pass
    OUT #0x10, A  // Newline

/////////////////////////////////////////////////////
// Test 9: SRAM  Register Load/Store   LMR / SMR   //
/////////////////////////////////////////////////////

    OUT #9, I   // Print 09                  
    OUT #0x20, A // Print ' '  
    OUT #0x78, A // Print 'x'
    OUT #0x4D, A // Print 'M'
    OUT #0x52, A // Print 'R'    
    OUT #0x3A, A // Print ':'   

    LDR R1, #0x05
    LDR R2, #0x20

    :store
    SMR R1, R2   // Store 5 in address 20
    OUT R1, A    // (20:0x05 , 21:0x04, 22:0x03, 23:0x02, 24:0x01) 
    OUT #0x20, A // Print ' ' 
    INC R2
    DEC R1
    BNE :store     // back to :Store 
    OUT #0x10, A // C9: Newline

    LDR R1, #0x05
    LDR R2, #0x24

    :load
    OUT R2, I     // (0x24, 0x23, 0x22, 0x21, 0x20)  
    OUT #0x3A, A  // Print ':' 
    LMR R3, R2    // Load address[R2] to R3
    OUT R3, A     // (1, 2, 3, 4, 5)  
    OUT #0x20, A  // Print ' ' 
    DEC R2
    DEC R1
    BNE :load     // back to Load 
    SUB R3, #5   // R3 = R3 - #5
    BEQ :test9_pass       // RELATIVE JUMP
    BRK           // Test 9 failed: R3 != #5
	:test9_pass
    OUT #0x10, A  // Newline

  
/////////////////////////////////////////////////////
// Test 10: PUSH/POP                               //
/////////////////////////////////////////////////////

    OUT #10, I   // Print 010                  
    OUT #0x20, A // Print ' '  
    OUT #0x53, A // Print 'S'
    OUT #0x54, A // Print 'T'
    OUT #0x43, A // Print 'C'    
    OUT #0x3A, A // Print ':'   

    LDR R1, #0xFF  // Stack max capacity
	// Push Loop
	:test10_push_loop
    PUSH R1
    DEC R1 
    BNE :test10_push_loop
	// Pop & Check loop
    INC R1
	:test10_pop_loop
    POP R2
    OUT #0x10, A  // Newline
    OUT #0x52, A // Print 'R'
    OUT #1, A   // Print '1'
    OUT #0x3A, A // Print ':'   
    OUT #0x20, A // Print ' '  
    OUT R1, I 
    OUT #0x20, A // Print ' '  
    OUT #0x20, A // Print ' '  
    OUT #0x20, A // Print ' '  
    OUT #0x52, A // Print 'R'
    OUT #2, A   // Print '2'
    OUT #0x3A, A // Print ':'   
    OUT #0x20, A // Print ' '  
    OUT R2, I 
  
    SUB R2, R2, R1
    BEQ :test9_check
    BRK     // Fail R1 != R2
	:test9_check
	INC R1 
    BNE :test10_pop_loop
	
/////////////////////////////////////////////////////
// Test 11: CALL function test                     //
/////////////////////////////////////////////////////

	OUT #11, I 		// Print 0B                  
    OUT NEW_LINE, A // Print ' '
	OUT	_C, A
	OUT _A, A
	OUT _L, A
	OUT _L, A
	OUT COLON, A
	OUT NEW_LINE, A
	//	Set Arguments
	//	R0 = hello_world(17, 76)
	LDR R1, #17
	LDR R2, #76
	OUT _H, A
	OUT _E, A
	OUT _L, A
	OUT _L, A
	OUT _O, A
	OUT SPACE, A
	OUT _W, A
	OUT _O, A
	OUT _R, A
	OUT _L, A
	OUT _D, A
	OUT LEFT_PAR, A
	OUT R1, I
	OUT COMMA, A
	OUT R2, I
	OUT RIGHT_PAR, A
	OUT NEW_LINE, A
	CALL :hello_world
	OUT _R, A
	OUT _E, A
	OUT _T, A
	OUT _U, A
	OUT _R, A
	OUT _N, A
	OUT _E, A
	OUT _D, A
	OUT COLON, A
	OUT SPACE, A
	OUT R0, I

// End of BIST    
    OUT #0x10, A  // Newline
    OUT #0x50, A // Print 'P'    
    OUT #0x41, A // Print 'A'    
    OUT #0x53, A // Print 'S'    
    OUT #0x53, A // Print 'S'    

    BRK  // (end of BIST) 
    
//	Test 11 Function Definition
//	Note: The CALL instuction overwrites the value in R0, can't be used as argument!
//	Return Value: R0
//	First Argument  (x): R1
//	Second Argument (y): R2
//	uint8_t hello_world(uint8_t x, uint8_t y) {
//		printf("Hello world(): &d &d\n", x, y);
//		return 2 * x + y;
//	}
:hello_world
	//	1.	printf("Hello world()\n");
	OUT _H, A
	OUT _E, A
	OUT _L, A
	OUT _L, A
	OUT _O, A
	OUT SPACE, A
	OUT _W, A
	OUT _O, A
	OUT _R, A
	OUT _L, A
	OUT _D, A
	OUT LEFT_PAR, A
	OUT RIGHT_PAR, A
	OUT NEW_LINE, A
	//	2.	printf(" x: %d y: %d\n", x, y);
	OUT SPACE, A
	OUT _X, A
	OUT COLON, A
	OUT SPACE, A
	OUT R1, I
	OUT SPACE, A
	OUT _Y, A
	OUT COLON, A
	OUT SPACE, A
	OUT R2, I
	OUT NEW_LINE, A
	//	3.	R0 = 2 * x + y
	ASL R0, R1
	ADD R0, R0, R2
	//	4.	return;
	RTS
