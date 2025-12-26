/*  Used Ben Eater's eeprom-programmer.ino file as a basis to write to this code; find the original here: https://github.com/beneater/eeprom-programmer/tree/master/eeprom-programmer */
/*  Ben Eater's code is MIT licensed, so we include the following copyright notice:
  Copyright (c) 2017 Ben Eater

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/


#define SHIFT_DATA 2
#define SHIFT_CLK 3
#define SHIFT_LATCH 4
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_EN 13
#define BASE_ADDR 0xA000 // 40,960 (out of 

// Print Macro voodoo
#define STRINGIZE_(x) #x
#define STRINGIZE(x) STRINGIZE_(x)

//*************************************************************************************************************

#define PAGE_SIZE 512
// #define ERASE_EEPROM
// #define SW_PROTECTION_REMOVAL

#define PROGRAMMING_DATA

//#define  PROG_NAME        Micro0_7
// #define  PROG_NAME        micro8_15
  #define  PROG_NAME        Micro16_23
// #define  BASE_PAGE        0

//#define  PROG_NAME        TwoLoops
#define  BASE_PAGE        0

// #define  PROG_NAME        FIBONACCI
// #define  BASE_PAGE        1

//#define  PROG_NAME        Multiply
//#define  BASE_PAGE        2

//#defome  PROG_NAME robertsonv2
//#define  PROG_NAME        display_test
//#define  BASE_PAGE        11
//#define BASE_PAGE 7
//#define  BASE_PAGE          8 
// #define TEST_EEPROM                // Erase the entire EEPROM and check success
// #define CHECK_CONTENT              // Read the entire EEPROM and print any non FF


//*************************************************************************************************************


/*
   Output the address bits and outputEnable signal using shift registers.
*/
void setAddress(unsigned int address, bool outputEnable) {
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80));
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address);

  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
}

char buf[80];

/****************************************************
 * readEEPROM 
 * Read a byte from the EEPROM at the specified address.
 */
byte readEEPROM(unsigned int address) 
{
  for (unsigned int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, INPUT);
  }
  setAddress(address, /*outputEnable*/ true);

  byte data = 0;
  for (unsigned int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
  }
  return data;
}


/****************************************************
 * writeEEPROM  
 * Write a byte to the EEPROM at the specified address.
 */
void writeEEPROM(unsigned int address, byte data) {

   
  sprintf(buf, "Programming address 0x%x  with data 0x%x", address, data);
  Serial.println(buf);

  setAddress(address, /*outputEnable*/ false);
  for (unsigned int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, OUTPUT);
  }

  for (unsigned int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }
  digitalWrite(WRITE_EN, LOW);
  delayMicroseconds(10);
  digitalWrite(WRITE_EN, HIGH);
  delay(10);
}


/****************************************************
 *   printContents
 *   Read the contents of one page (256 bytes) from the EEPROM and print them to the serial monitor.
 */
void printContents(unsigned int page_addr = 0) 
{
  for (unsigned int base = page_addr; base < PAGE_SIZE+page_addr; base += 16) {
    byte data[16];
    for (unsigned int offset = 0; offset <= 15; offset += 1) {
      data[offset] = readEEPROM(base + offset);
    }

    sprintf(buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}


/****************************************************
 *   checkContents
 *   Search the entire EEPROM for evidance of life
 *   Print any byte with zeros.
 */
void checkContents() 
{
  unsigned int data;
  unsigned int maxPrint = 0;
  for (unsigned int i = 0; i <= 32767; i++)
  {
      data = readEEPROM(i);
      if (0xff != data)
      {
         sprintf(buf, "Address 0x%x(%d):  Data 0x%x", i, i, data);
         Serial.println(buf);
         maxPrint++;
      }
      if (i % 512 == 0) 
      {
         Serial.print(".");
      }
      if(maxPrint > 30) break;
  }
  Serial.println(" Done");
}

/****************************************************
 * testEEPROM
 * Write over the entire EEPROM in attempt to find life
 */
 
void testEEPROM() 
{
  unsigned int data;
  unsigned int maxPrint = 0;
  for (unsigned int i = 0; i <= 32767; i++)
  {   
      writeEEPROM(i, 0x22);
      data = readEEPROM(i);
      if (0xff != data)
      {
         sprintf(buf, "Address 0x%x(%d):  Data 0x%x", i, i, data);
         Serial.println(buf);
         maxPrint++;
      }
      if (i % 512 == 0) 
      {
         Serial.print(".");
      }
      if(maxPrint > 30) break;
  }
  Serial.println(" Done");
}


// Micro-instruction LOW EEPROM

byte Micro0_7[] = {
    0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  00000 - NOP
    0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  00001 - BRK
    0x12, 0x19, 0x24, 0x80, 0x00, 0x00, 0x00, 0x00, //  00010 - ALU
    0x60, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  00011 - Load Immediate to Reg
    0x30, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  00100 - MOV
    0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  00101 - BRANCH (Absolute)
    0x12, 0x41, 0x24, 0x80, 0x00, 0x00, 0x00, 0x00, //  00110 - ALU Immediate
    0x02, 0x41, 0x04, 0x80, 0x00, 0x00, 0x00, 0x00, //  00111 - BRANCH (Relative)
    0x00, 0x10, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, //  01000 - OUT   (DEPRECATABLE)
    0x40, 0x20, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, //  01001 - Load Memory Absolute
    0x40, 0x10, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, //  01010 - Store Memory Absolute
    0x10, 0x20, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, //  01011 - Load Memory Register
    0x10, 0x10, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, //  01100 - Store Memory Register
    0x00, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, //  01101 - OUT immediate
    0x00, 0x10, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, //  01110 - PUSH
    0x20, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  01111 - POP
    0x00, 0x00, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00, //  10000 - JSR
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, //  10001 - RTS
    0x12, 0x19, 0x04, 0x80, 0x00, 0x00, 0x00, 0x00, //  10010 - TST
    0x12, 0x41, 0x04, 0x80, 0x00, 0x00, 0x00, 0x00, //  10011 - TST Immediate
    0x40, 0x20, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, //  10100 - LDWL (Load Data Word "Low" for ARSrc values PC and SP)
    0x40, 0x20, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, //  10101 - LDWH (Load Data Word "High" for ARSrc values DP and BP)
    0x40, 0x10, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, //  10110 - SDWL (Store Data Word "Low" for ARSrc values PC and SP)
    0x40, 0x10, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, //  10111 - SDWH (Store Data Word "High" for ARSrc values DP and BP)
    0x40, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, //  11000 - MAAL (Move Address Register to Address Register for ARSrc values PC and SP)
    0x40, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, //  11001 - MAAH (Move Address Register to Address Register for ARSrc values DP and BP)
    0x00, 0x20, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, //  11010 - MAG (Move Address Register to General Purpose Register)
    0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  11011 - LDA (Load Immediate to Address Register)
    0x10, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  11100 - MGA (Move General Purpose Register to Address Register)
    0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  11101 - NOP
    0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  11110 - NOP
    0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 //  11111 - NOP
    };

// Micro-instruction High EEPROM
byte micro8_15[] = { 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  00000 - NOP
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  00001 - BRK
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  00010 - ALU
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  00011 - Load Immediate to Reg
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  00100 - MOV
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  00101 - BRANCH
    0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  00110 - ALU immediate
    0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, //  00111 - BRANCH (Relative
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  01000 - OUT   (DEPRECATABLE)
    0x10, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  01001 - Load Memory Absolute
    0x10, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  01010 - Store Memory Absolute
    0x10, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  01011 - Load Memory Register
    0x10, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  01100 - Store Memory Register
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  01101 - OUT immediate
    0x80, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  01110 - PUSH
    0xc4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  01111 - POP
    0x4A, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, //  10000 - JSR
    0x80, 0xc5, 0xc5, 0x80, 0x00, 0x00, 0x00, 0x00, //  10001 - RTS
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  10010 - TST
    0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  10011 - TST Immediate
    0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  10100 - LDWL (Load Data Word "Low" for ARSrc values PC and SP)
    0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  10101 - LDWH (Load Data Word "High" for ARSrc values DP and BP)
    0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  10110 - SDWL (Store Data Word "Low" for ARSrc values PC and SP)
    0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  10111 - SDWH (Store Data Word "High" for ARSrc values DP and BP)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  11000 - MAAL (Move Address Register to Address Register for ARSrc values PC and SP)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  11001 - MAAH (Move Address Register to Address Register for ARSrc values DP and BP)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  11010 - MAG (Move Address Register to General Purpose Register)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  11011 - LDA (Load Immediate to Address Register)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  11100 - MGA (Move General Purpose Register to Address Register)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  11101 - NOP
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  11110 - NOP
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  //  11111 - NOP
  };
// Micro-instruction High EEPROM
byte Micro16_23[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  00000 - NOP
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  00001 - NOP
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  00010 - NOP
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  00011 - NOP
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  00100 - NOP
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  00101 - NOP
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  00110 - NOP
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  00111 - NOP
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  01000 - NOP
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  01001 - NOP
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  01010 - NOP
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  01011 - NOP
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  01100 - NOP
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  01101 - OUT immediate
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  01110 - PUSH
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  01111 - POP
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  10000 - JSR
    0x01, 0x06, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, //  10001 - RTS
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  10010 - TST
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  10011 - TST Immediate
    0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  10100 - LDWL (Load Data Word "Low" for ARSrc values PC and SP)
    0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  10101 - LDWH (Load Data Word "High" for ARSrc values DP and BP)
    0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  10110 - SDWL (Store Data Word "Low" for ARSrc values PC and SP)
    0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  10111 - SDWH (Store Data Word "High" for ARSrc values DP and BP)
    0x08, 0x10, 0x32, 0x66, 0x00, 0x00, 0x00, 0x00, //  11000 - MAAL (Move Address Register to Address Register for ARSrc values PC and SP)
    0x08, 0x10, 0x32, 0x66, 0x00, 0x00, 0x00, 0x00, //  11001 - MAAH (Move Address Register to Address Register for ARSrc values DP and BP)
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  11010 - MAG (Move Address Register to General Purpose Register)
    0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  11011 - LDA (Load Immediate to Address Register)
    0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  11100 - MGA (Move General Purpose Register to Address Register)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  11101 - NOP
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  11110 - NOP
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  11111 - NOP
  };

// Programs

// Hierarchical Loops


byte TwoLoops[] = {   
  
0x43,  0x20,  // LDR R1, #32  // Space
0x83,  0x00,  // LDR R2, #0
0xc3,  0x00,  // LDR R3, #0
// Start loop
0x04,  0x30,  // MOV R0, R3
0x06,  0x32,  // AND R0, #03
0xE7,  0x02,  // BNE - skip new line
0x03,  0x10,  // LDR R0, #16  // New Line
0x08,  0x00,  // OUT R0, A // Newline
0x28,  0x20,  // OUT R2, I
0x28,  0x30,  // OUT R3, I
0x08,  0x10,  // OUT R1 Space
0xc6,  0x16,  // ADD R3, R3, #01
0xe5,  0x03,  // BNE #3
0x86,  0x16,  // ADD R2, R2, #01
0xe5,  0x03,  // BNE, #3
0x28,  0x20,  // OUT R2, I
0x28,  0x30,  // OUT R3, I
0x01,  0x00,  // BRK

};



// FIBONACCI WITH BRANCHES (do-while loop) NEW BRANCH INSTRUCTION (Capable of performing NOT)
byte FIBONACCI[] = {
    0x03, 0x00, //  0 LDR R0, 0x00
    0x43, 0x01, //  1 LDR R1, 0x01
    0x02, 0x16, //  2 ADD R0, R0, R1
    0x45, 0x09, //  3 If carry go to address 9
    0x28, 0x00, //  4 OUT R0->H
    0x42, 0x16, //  5 ADD R1, R0, R1
    0x45, 0x09, //  6 Branch if Carry, to address 9
    0x18, 0x10, //  7 OUT R1->L
    0x05, 0x02, //  8 Unconditional Branch to address 2
    0x01, 0x00  //  9 BRK
};


// ROBERTSON'S MULTIPLICATION UNSIGNED version 2 (with rotate right)
byte Multiply[] = {
    0x43, 0x23,   //  0 LDR R1, #23 
    0x83, 0x41,   //  1 LDR R2, #41 --> Expect 08e3
    
    //:Loop
    0x4A, 0x01,   // 2 SMA R1, #0x01 
    0x8A, 0x02,   // 3 SMA R2, #0x02 
  
    0x28, 0x10,   // 4 OUT R1, I
    //0x03, 0x20,   // LDR R0, #0x20 // Space
    //0x08, 0x00,   // OUT R0, A
    0x0D, 0x20,   // 5 OUT ' '
    //0x03, 0x2A,   // LDR R0, #0x2A // '*'
    //0x08, 0x00,   // OUT R0, A
    0x0D, 0x2A,   // 6 OUT '*'
    //0x03, 0x20,   // LDR R0, #0x20 // Space
    //0x08, 0x00,   // OUT R0, A
    0x0D, 0x20,   // 7 OUT ' '
    0x28, 0x20,   // OUT R2, I
    0x0D, 0x20,   // OUT ' '
    //0x08, 0x00,   // OUT R0, A
    //0x03, 0x3D,   // LDR R0, #0x3D // '='
    //0x08, 0x00,   // OUT R0, A
    0x0D, 0x3D,   // OUT '='
    //0x03, 0x20,   // LDR R0, #0x20 // Space
    //0x08, 0x00,   // OUT R0, A
    0x0D, 0x20,   // OUT ' '
    
    0x03, 0x00,   //  0 LDR R0, #0
    0xc3, 0x08,   //  3 LDR R3, #8  (0x08)
    0x46, 0x0a,   //  4 LSR R1 (using immediate opcode)
    0xc7, 0x01,   //  5 BNC + 1 (relative jump over 1 instruction)
    0x02, 0x26,   //  6 ADD R0, R0, R2
    0x46, 0x08,   //  7 LSL R1
    0x06, 0x0a,   //  8 LSR R0
    0x46, 0x0c,   //  9 ROR R1
    0xc6, 0x17,   //  C SUB R3, #1
    0xe7, 0xf8,   //  D BNZ 4 (relative jump back 9 instructions by substracting 8 from PC)
    0x28, 0x00,   //  A OUT R0, I 
    0x28, 0x10,   //  B OUT R1, I
    0x03, 0x10,   // LDR R0, #0x10 // New line
    0x08, 0x00,   // OUT R0, A
    0x49, 0x01,   // LMA R1, #01
    0x89, 0x02,   // LMA R2, #02
    0x86, 0x17,   // SUB R2, #01
    0x67, 0x03,   // BEQ end
    0x46, 0x17,   // SUB R1, #01
    0x67, 0x01,   // BEQ end
    0x05, 0x02,   // JMP loop
// :End
    0x01, 0x00    //  E BRK
};


// ROBERTSON'S MULTIPLICATION SIGNED version 3 (with rotate right)
byte robertsonv3[] = {
    0x03, 0x00,   //  0 LDR R0, #0
    0x43, 0xd6,   //  1 LDR R1, #D6 (-42) //#80 (-128) 
    0x83, 0x17,   //  2 LDR R2, #17 (23) --> Expect the value FC3A //#1 (1) --> Expect 0x0080 //--> Expect 0xF71D (-2275)
    0xc3, 0x07,   //  3 LDR R3, #7  (0x07)
    0x46, 0x0a,   //  4 LSR R1  (Shift right using immediate opcode R1[0] -> Carry)
    0xc7, 0x01,   //  5 BNC + 1 (relative jump over 1 instruction)
    0x02, 0x26,   //  6 ADD R0, R0, R2
    0x46, 0x08,   //  7 LSL R1  (resume R1 position)
    0x06, 0x0b,   //  8 ASR R0  (Shift Right: R0 -> Carry -> R1) 
    0x46, 0x0c,   //  9 ROR R1
    0x28, 0x00,   //  A OUT R0 H
    0x18, 0x10,   //  B OUT R1 L
    0xc6, 0x17,   //  C SUB R3, #1
    0xe7, 0xf6,   //  D BNZ 4 (relative jump back 9 instructions by substracting 10 from PC)
    0x46, 0x0a,   //  E LSR R1  (Shift right using immediate opcode R1[0] -> Carry)
    0xc7, 0x01,   //  F BNC + 1 (relative jump over 1 instruction)
    0x02, 0x87,   //  10 SUB R0, R0, R2  R0 = R0 - R2
    0x46, 0x08,   //  11 LSL R1  (resume R1 position)
    0x06, 0x0b,   //  12 ASR R0  (Shift Right: R0 -> Carry -> R1) 
    0x46, 0x0c,   //  13 ROR R1
    0x28, 0x00,   //  14 OUT R0 H
    0x18, 0x10,   //  15 OUT R1 L
    0x01, 0x00    //  16 BRK
};


// RAM TEST
// 0 LDR R0, #5A
// 1 LDR R1, #12
// 2 LDR R2, #92
// 3 LDR R3, #DE
// 4 SMA R0, #00 // Store 5A in address 00
// 5 SMA R1, #01 // Store 12 in address 01
// 6 SMA R2, #02 // Store 92 in address 02
// 7 SMA R3, #03 // Store DE in address 03
// 8 LMA R3, #00 // Load value at address 00 into R3 (R3 = 5A)
// 9 LMA R2, #01 // Load value at address 01 into R2 (R2 = 12)
// A LMA R1, #02 // Load value at address 02 into R1 (R1 = 92)
// B LMA R0, #03 // Load value at address 03 into R0 (R0 = DE)
// C OUT R0 High (DE)
// D OUT R1 LOW  (92)
// E OUT R2 HIGH (12)
// F OUT R3 LOW  (5A)
byte ram1[] = {
  0x03, 0x5A, // 0 LDR R0, #5A
  0x43, 0x12, // 1 LDR R1, #12
  0x83, 0x92, // 2 LDR R2, #92
  0xC3, 0xDE, // 3 LDR R3, #DE
  0x0A, 0x00, // 4 SMA R0, #00 // Store 5A in address 00
  0x4A, 0x01, // 5 SMA R1, #01 // Store 12 in address 01
  0x8A, 0x02, // 6 SMA R2, #02 // Store 92 in address 02
  0xCA, 0x03, // 7 SMA R3, #03 // Store DE in address 03
  0xC9, 0x00, // 8 LMA R3, #00 // Load value at address 00 into R3 (R3 = 5A)
  0x89, 0x01, // 9 LMA R2, #01 // Load value at address 01 into R2 (R2 = 12)
  0x49, 0x02, // A LMA R1, #02 // Load value at address 02 into R1 (R1 = 92)
  0x09, 0x03, // B LMA R0, #03 // Load value at address 03 into R0 (R0 = DE)
  0x28, 0x00, // C OUT R0 High (DE)
  0x18, 0x10, // D OUT R1 LOW  (92)
  0x28, 0x20, // E OUT R2 HIGH (12)
  0x18, 0x30, // F OUT R3 LOW  (5A)
  0x01, 0x00  // 10 BREAK
};

// DISPLAY TEST (PRINT "M")
// LDR R0, #38 (Function Set)
// LDR R1, #0F (Set Display ON)
// REMOVED LDR R2, #01 (Clear Display)
// LDR R2, #48
// LDR R3, #4D ('M')
// OUT R0, INSTRUCTION
// OUT R1, INSTRUCTION
// OUT R2, INSTRUCTION
// OUT R3, DATA
// BRK
byte display_test[] = {
  0x03, 0x38, // LDR R0, #38 (Function Set)
  0x43, 0x0F, // LDR R1, #0F (Set Display ON)
  0x83, 0x48, // LDR R2, #48 ('H')
  0xC3, 0x45, // LDR R3, #45 ('E')
  0x08, 0x00, // OUT R0, INSTRUCTION
  0x08, 0x10, // OUT R1, INSTRUCTION
  0x28, 0x20, // OUT R2, DATA
  0x28, 0x30, // OUT R3, DATA
  0x01, 0x00,  // BRK
};



void setup() {
  // put your setup code here, to run once:
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);
  Serial.begin(57600);

//*************************************************************************************************************
//*************************************************************************************************************

  // Erase entire EEPROM 

#ifdef ERASE_EEPROM
  Serial.print("Erasing EEPROM");
  for (unsigned int address = 0; address <= 2047; address += 1) {
    writeEEPROM(address, 0x00);

    if (address % 64 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" done");
#else
  Serial.println("Skipping EEPROM erasure");
#endif

//*************************************************************************************************************

  // Remove Software Data Protection
#ifdef SW_PROTECTION_REMOVAL
  Serial.println("Software Data Protection Removal");

  writeEEPROM(0x5555, 0xAA);   //1. Write 0xAA to 0x5555 
  writeEEPROM(0x2AAA, 0x55);   //2. Write 0x55 to 0x2AAA
  writeEEPROM(0x5555, 0x80);   //3. Write 0x80 to 0x5555 
  writeEEPROM(0x5555, 0xAA);   //4. Write 0xAA to 0x5555 
  writeEEPROM(0x2AAA, 0x55);   //5. Write 0x55 to 0x2AAA
  writeEEPROM(0x5555, 0x20);   //6. Write 0x20 to 0x5555
  writeEEPROM(0x0033, 0x33);   //7. Write 0x33 to 0x0033  // My test   
  writeEEPROM(0x0033, 0x33);   //8. Write 0x33 to 0x0033  // My test   
  printContents();    //9. Read  address 0x0033
#else
  Serial.println("Skipping Software Data Protection Removal");
#endif

//*************************************************************************************************************

#ifdef PROGRAMMING_DATA

  // Program data bytes
  unsigned int add_zeros = 256;
  unsigned int address = 0;
  sprintf(buf, "Programming  %d bytes, starting from %x with Program: " STRINGIZE(PROG_NAME) , 
         sizeof(PROG_NAME), (PAGE_SIZE * BASE_PAGE));
  Serial.println(buf);
  
  for (address = 0; address < sizeof(PROG_NAME); address += 1)
  {
    writeEEPROM((PAGE_SIZE * BASE_PAGE) + address, PROG_NAME[address]);
  }
  for (address = sizeof(PROG_NAME); address < add_zeros + sizeof(PROG_NAME); address++)
  {
    writeEEPROM((PAGE_SIZE * BASE_PAGE) + address, 0);
  }
  
  Serial.println(" done");

  printContents(PAGE_SIZE * BASE_PAGE);
#else
  Serial.println("Skipping Programming Data");
#endif
//*************************************************************************************************************


#ifdef TEST_EEPROM
  Serial.println("Testing if EEPROM is writeable");
  testEEPROM();
#endif
//*************************************************************************************************************

#ifdef CHECK_CONTENT
  Serial.println("Checking if EEPROM contain any data");
  checkContents();
#endif

//*************************************************************************************************************

#if 0
// Test few ops for logic analyzer
    unsigned int data;
    unsigned int address;
    address = 0x1001;
    data = readEEPROM(address);
    sprintf(buf, "Read Address 0x%x(%d):  Data 0x%x", address, address, data);
    Serial.println(buf);
    address = 0x1002;
    data = readEEPROM(address);
    sprintf(buf, "Read Address 0x%x(%d):  Data 0x%x", address, address, data);
    Serial.println(buf);
    
    data = 0x22;
    address = 0x1001;
    sprintf(buf, "Write Address 0x%x(%d):  Data 0x%x", address, address, data);
    writeEEPROM(address, data);
    
    data = 0x23;
    address = 0x1002;
    sprintf(buf, "Write Address 0x%x(%d):  Data 0x%x", address, address, data);
    writeEEPROM(address, data);
   
    address = 0x1001;
    data = readEEPROM(address);
    sprintf(buf, "Read Address 0x%x(%d):  Data 0x%x", address, address, data);
    Serial.println(buf);
 
    address = 0x1002;
    data = readEEPROM(address);
    sprintf(buf, "Read Address 0x%x(%d):  Data 0x%x", address, address, data);
    Serial.println(buf);
   
#endif



}


void loop() {
  // put your main code here, to run repeatedly:

}
