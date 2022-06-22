//////////////////////////////////
// Test 1: Shift Left (8: ASL)  //
//////////////////////////////////
OUT #01, I   // Print 01                  
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
SUB R3, R3, R2           (carry is set)   
BEQ #01       // RELATIVE JUMP
BRK           // Test 1 failed
ADD R3, #0x01 // (reset carry)
OUT #0x10, A     // Newline

//////////////////////////////////
// Test 2: Rotate Left (9: ROL) //
//////////////////////////////////
OUT #02, I   // Print 02                  
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
BCS #01       // RELATIVE JUMP
BRK           // Test 2 failed: Carry was not set
LDR R3, #0x41 
SUB R3, R3, R2
BEQ #01       // RELATIVE JUMP
BRK           // Test 2 failed: Result is not 0x41    
OUT #0x10, A  // Newline
    
///////////////////////////////////////////
// Test 3: Logic Shift Right  (A: LSR)   //
///////////////////////////////////////////
OUT #03, I   // Print 03                  
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
BCS #01       // Relative
BRK           // Test 3 failed: Carry was not set
LDR R3, #0x21 
SUB R3, R3, R2
BEQ #01       // relative
BRK           // Test 3 failed: Result is not 0x21    
OUT #0x10, A  // Newline

//////////////////////////////////////////////
// Test 4: Arithmetic Shift Right  (B: ASR) //
//////////////////////////////////////////////
OUT #04, I   // Print 04                  
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
BCS #01       // Relative
BRK           // Test 4 failed: Carry was not set
LDR R3, #0xE1 
SUB R3, R3, R2
BEQ #01       // Relative
BRK           // Test 4 failed: Result is not 0xE1    
OUT #0x10, A  // Newline

//////////////////////////////////////////////
// Test 5: Add 0x1234 + 0x6DCD =  0x8001    //
//////////////////////////////////////////////
 
    OUT #05, I   // Print 05                  
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
    
    BCS #01      // Relative
    BRK          // Test 5 failed: Carry was not set
    LDR R2, #0x12
    LDR R1, #0x6D
    ADC R3, R2, R1  // 0x80 = 0x12 + 0x6D + Carry  
    OUT R3, I    //       (0x80)
    BCC #01      // Relative
    BRK          // Test 5 failed: Carry was  set
    LDR R2, #0x80
    SUB R3, R3, R2
    BEQ #01      // Relative
    BRK          // Test 5 failed: Result is not 0x80    
    OUT #0x10, A  // Newline

//////////////////////////////////////////////
// Test 6: Sub (0xA765 – 0x34AB) = 0x72BA   //  
//////////////////////////////////////////////

    OUT #06, I   // Print 06                  
    OUT #0x20, A // Print ' '  
    OUT #0x53, A // Print 'S'
    OUT #0x55, A // Print 'U'
    OUT #0x42, A // Print 'B'    
    OUT #0x3A, A // Print ':'   

    LDR R2, #0x65
    LDR R1, #0xAB
    SUB R3, R2, R1    // 65 - AB =  0x + Carry
    OUT R3, I         // (0xBA) (No carry, means borrow)
    OUT #0x20, A // Print ' '  
    BCC #01      // Relative
    BRK          // Test 6 failed: Carry was set
    LDR R2, #0xA7
    LDR R1, #0x34
    SBC R3, R1, R2    // R2 - R1 + Cin = A7 - 34 + Cin = 72 + Cout
    OUT R3, I         //  (0x72)
    BCS #01           // Relative
    BRK               // Test 6 failed: Carry was not set
    LDR R2, #0x72
    SUB R3, R3, R2
    BEQ #01      // Relative
    BRK          // Test 6 failed: Result is not 0x72       
    OUT #0x10, A  // Newline

////////////////////////////////////
// Test 7: Rotate Right (C: ROR)  //
////////////////////////////////////

    OUT #07, A   // Print 7                  
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
    BCS #01      // RELATIVE JUMP
    BRK          // Test 7 failed: Carry was not set
    LDR R3, #0x61 
    SUB R3, R3, R2
    BEQ #01      // RELATIVE JUMP
    BRK          // Test 7 failed: Result is not 0x61    
    OUT #0x10, A  // Newline

/////////////////////////////////////////////////////
// Test 8: SRAM  Immediate Load/Store   LMA / SMA  //
/////////////////////////////////////////////////////

    OUT #08, I   // Print 08                  
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
    BEQ #01       // RELATIVE JUMP
    BRK           // Test 8 failed: Result is not 0x00    
    OUT #0x10, A  // Newline

/////////////////////////////////////////////////////
// Test 9: SRAM  Register Load/Store   LMR / SMR   //
/////////////////////////////////////////////////////

    OUT #09, I   // Print 09                  
    OUT #0x20, A // Print ' '  
    OUT #0x78, A // Print 'x'
    OUT #0x4D, A // Print 'M'
    OUT #0x52, A // Print 'R'    
    OUT #0x3A, A // Print ':'   

    LDR R1, #0x05
    LDR R2, #0x20

    //:store
    SMR R1, R2   // Store 5 in address 20
    OUT R1, A    // (20:0x05 , 21:0x04, 22:0x03, 23:0x02, 24:0x01) 
    OUT #0x20, A // Print ' ' 
    INC R2
    DEC R1
    BNE #0xFA     // back to :Store 
    OUT #0x10, A // C9: Newline

    LDR R1, #0x05
    LDR R2, #0x24

    //:load
    OUT R2, I     // (0x24, 0x23, 0x22, 0x21, 0x20)  
    OUT #0x3A, A  // Print ':' 
    LMR R3, R2    // Load address[R2] to R3
    OUT R3, A     // (1, 2, 3, 4, 5)  
    OUT #0x20, A  // Print ' ' 
    DEC R2
    DEC R1
    BNE #0xF8     // back to Load 
    SUB R3, #05   // R3 = R3 - #5
    BEQ #01       // RELATIVE JUMP
    BRK           // Test 9 failed: R3 != #5
    OUT #0x10, A  // Newline

// End of BIST    
    OUT #0x50, A // Print 'P'    
    OUT #0x41, A // Print 'A'    
    OUT #0x53, A // Print 'S'    
    OUT #0x53, A // Print 'S'    
  
    BRK  // (end of BIST) 
    