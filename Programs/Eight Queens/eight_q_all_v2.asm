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

//////////////////////////////////
// Enhance to 16bit address range
// 0x0000 .. 0x1FFF  ROM 
// 0x2000 .. 0xDFFF  RAM
// 0xE000 .. 0xFFFF  Registers (MMIO)
// Data Pointer:  0x4000 Fixed
// Stack Pointer: 0x4000 Variale (Stack is not used. )
// Variables:
//		4000..4007 Address represents Row i; Value represents the column
//		4009 - Solution counter
//      R0 - current row [0..7]
//////////////////////////////////

//                                            +----------+
//                                            |  Setup   | 
//                                            +----------+
//                                                 |
//                                                 |
//                      +-----------------------+  |  +------------------------+
//                      |                       |  |  |                        |
//                      |                       V  V  V                        |
//                      |                +-------------------+                 |
//                      |                | Check if current  |                 |
//                      |                | setting is valid  |                 |
//                      |                | :for_loop_start   |                 |
//                      |                +-------------------+                 |
//                      |                  |               |                   |
//                      |     :not_valid   V               V  :valid           | 
//                      |    +---------------+          +--------------+       |
//             +--------|--->| Move Queen up |          | Add Queen    |       |
//             |        |    | to next column|          | to next Row  |       |
//             |        |    +---------------+          +--------------+       |
//             |        |            |                         |               |
//             |        |    +---------------+          +---------------+      |
//             |        | No | Queen outside?|          |Went out board?|  No  |
//             |        +----+ (Column > 7)  |          | (Row>7)       +------+
//             |             +---------------+          +---------------+      
//             |                     |  Yes                    | Yes
//             |                     |                         |
//             |                     V :go_back_one_row        V  :print_solution
//             |             +---------------+          +----------------+        
//             |             | Go row back   |<---------+ Print Solution |   
//             |             +---------------+          +----------------+      
//             |                     |
//     :move_up_one_column           V   
//             |             +---------------+ 
//             |   No        |Went out board?|
//             +-------------+  (Row<0)      |
//                           +---------------+ 
//                                   |
//                                   V   Yes
//                           +---------------+ 
//                           |   THE END     |
//                           +---------------+ 
//                      

.define 2 counter_base	0xE200

LDA %DP, H, counter_base[1]
LDA %DP, L, counter_base[0]
SDW R0, %DP, #0     // Reset Counter


// Set Stack Pointer to 0x4000
LDA %SP, H, #0x40  
LDA %SP, L, #0x00


// Set Data Pointer to  0x4000
LDA %DP, H, #0x40  
LDA %DP, L, #0x00

// Variables Initalization 
// Prepare the board with a single queen at 0,0
// Last Row:
//        L_Row index is R0
//        L_Row_pointer is DP
//        L_Column is *DP

LDR R0, #0			// L_Row R0 = 0
SDW R0, %DP, #0		// SMR R0, R0	// *L_Row_pointer = 0 // board[0] = 0
SDW R0, %DP, #9		// SMA R0, #9  // num_solutions = 0


:for_loop_start // (DP,R0 point to the last row)
///////////////
// Input Params:
//			DP - Last Column
//			R0 - Last Column Index
//
// Internal varaibles:
// Scan Row:
//        S_Row index is R1
//        S_Row_pointer is SP
//        S_Column is *SP

MOV R1, R0			// S_Row R1 = L_Row R0
LDW R2, %DP, #0		// LMR R2, R0	// L_Column R2 = L_Row_pointer *DP

MAA %SP, %DP, #-1	// S_Row_pointer = L_Row_pointer - 1
DEC R1				// S_Row = L_Row - 1

// Check current queen backward with all existing queens

:for_loop_check
///////////////
BMI :valid 	// if out of board (S_Row = -1), all columns checked ok. Branch to :valid 

// for_loop_body

LDW R3, %SP, #0		// LMR R3, R1	// S_Column R3 = S_Row_pointer *SP

// Horizontal check

SUB R3, R3, R2	// R3 = board[i] - col  S_Column = S_Column - L_Column
BEQ :not_valid		//Both Queens are on the same Column

// Diagonal check 1

SUB R3, R3, R0		// R3 = S_Column - L_Column - L_Row
ADD R3, R3, R1		// R3 = (S_Column - L_Column) + (S_Row - L_Row)
BEQ :not_valid		// if (R3==0) Scan_Queen and Last_Queen are on the same diagonal

// Diagonal check 2

LDW R3, %SP, #0		// Restore S_Column R3 = S_Row_pointer *SP

SUB R3, R3, R2		// S_Column = S_Column - L_Column
ADD R3, R3, R0		// R3 = S_Column - L_Column + L_Row  
SUB R3, R3, R1		// R3 = (S_Column - L_Column) - (S_Row - L_Row) 
BEQ :not_valid		// if (R3==0) Scan_Queen and Last_Queen are on the same diagonal

// for_loop_update
DEC R1              // S_Row--
MAA %SP, %SP, #-1	// S_Row_pointer--
JMP :for_loop_check // goto :for_loop_check

////////////////////////////////////////////////////////////////

:valid
////// Move to the next row

INC R0				// L_Row++
MAA %DP, %DP, #1	// L_Row_pointer++
LDR R2, #0
SDW R2, %DP, #0		// *L_Row_pointer = 0
TST R0, #7
BMI :for_loop_start   // Covers rows 0..6
BEQ :for_loop_start   // Covers row 7

// If passed las row, a solution is found

JMP :print_solution

// return_from_print

JMP :go_back_one_row

///////////////////////////////////////////////////////

:not_valid
:move_up_one_column

// Move to next column

LDW R2, %DP, #0		// R2 = board[row]
INC R2				
SDW R2, %DP, #0		// board[row] = board[row]  + 1

// Check if column went outside the board
TST R2, #7
BMI :for_loop_start		// if board[row] < 7, go to :for_loop_start
BEQ :for_loop_start		// if board[row] = 7, go to :for_loop_start

// Go Back one Row
:go_back_one_row

MAA %DP, %DP, #-1	// L_Row_pointer--
DEC R0				// L_Row--
BMI :end			// If L_Row < 0, goto end
JMP :move_up_one_column

////////////////////////////////
:print_solution
 
LDA %SP, H, #0x40  
LDA %SP, L, #0x00
LDW R3, %SP, #9 // LMA R3, #9  // R3 = num_solutions
INC R3			// R3 = num_solutions + 1
SDW R3, %SP, #9	// SMA R3, #9  // num_solutions = num_solutions + 1
OUT R3, I       // Print solution number

OUT COLON, A    // Print ':'
OUT SPACE, A    // Print ' '
LDR R1, #65	// R1 = 'A'
LDR R2, #7 // Loop counter

:print_loop 

OUT R1, A	// Print rank (letter)
LDW R3, %SP, #0
INC R3      // Increment the number (print 1-indexed)
OUT R3, A	// Print number (row)

MAA %SP, %SP, #1	
INC R1		// R1++
DEC R2
BPL :print_loop		// if R2 > 0, go to print_solution
OUT NEW_LINE, A	// Print '\n'
JMP :go_back_one_row

///////////////////////////////


:end
LDA %DP, H, counter_base[1]
LDA %DP, L, counter_base[0]
SDW R0, %DP, #1  // Latch Counter

OUT _C, A
OUT _L, A
OUT _K, A
OUT COLON, A
OUT SPACE, A

LDW R0, %DP, #5  // Print Byte4
OUT R0, I
LDW R0, %DP, #4  // Print Byte3
OUT R0, I
LDW R1, %DP, #3  // Print Byte2
OUT R1, I
LDW R2, %DP, #2  // Print Byte1
OUT R2, I
OUT DOT, A	// Print '.'
BRK
