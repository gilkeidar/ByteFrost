#include <SD.h>
#include <sam.h>

//  ===== CONSTANTS =====

//  === CODE MODIFICATION ===
#define DEBUG 0

#define SECTOR_SIZE 256

//  === PIN DEFINITIONS for MKRZERO Ardoino Wrapper ===
// These numbers show on the PCB
//  Data Bus (RAM read / write data) PORTD (digital pins 0 - 7)
#define GRANT_INT   8   //  Bus Grant interrupt (incoming) on digital pin 8 (A1)
#define CNT         9   //  Counter count signal (outgoing) on digital pin 9 (A2)
#define RST         10  //  Counter reset signal (outgoing) on digital pin 10 (A3)
#define SERIAL_IN   11  //  Serial data (incoming) on digital pin 11 (A4)
#define SHIFT       12  //  Shift register shift signal (outgoing) on digital pin 12 (A5)
#define BUS_RELEASE 15  //  Bus arbitration release signal (returns controls of the busses to the ByteFrost)

//  === PIN DEFINITIONS for SAMD21 CPU Direct Register Access ===
// Through PORT A and PORT B (each 32 bits)

uint RST_PA19 = 1 << 19;
uint CNT_PA17 = 1 << 17;
uint WE_INV_PB02 = 1 << 2;
uint SRAM_OE_INV_PB03 = 1 << 3;

//  ===== FUNCTION DECLARATIONS
void BusGrant();

//  ===== GLOBAL VARIABLES  =====

// Global buffer that serves the four operations:
//        SDRead:     SD -> Buffer
//        SDWrite:    Buffer -> SD
//        memoryRead: Memory -> Buffer
//        memoryWrite: Buffer -> Memory
char sectorDataBuffer[SECTOR_SIZE] = { 0 };

//  Digital pins for data bus (PORTD)
pin_size_t dataBus[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };

//  SDCard information code from https://docs.arduino.cc/learn/programming/sd-guide
Sd2Card card;
SdVolume volume;
SdFile root;

//  ByteFrost disk file
File disk;

//  ByteFrost config file
File config;

//  ByteFrost disk file name (default)
const int MAX_DISK_NAME_LEN = 1024;
const String BYTEFROST_DEFAULT_DISK_NAME = "disk";

//  ByteFrost config file name
const String BYTEFROST_CONFIG_FILE_NAME = "bf.con";

//  Size of ByteFrost disk in KB
const uint32_t BYTEFROST_DISK_SIZE = 512;
const int chipSelect = SDCARD_SS_PIN;

//  ===== SD CARD FUNCTIONS =====
/**
 *  Initializes SD Card (using SD.h interface and direct Sd2Card interface).
 *
 *  @returns 1 if successful, and 0 otherwise
 */
int initialize() {
  //  Setup SD Card
  #if DEBUG
    Serial.println("Setting up SD card...");
  #endif

  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    #if DEBUG
      Serial.println("Error: Failed to initialize the SD library and card.");
    #endif

    return 0;
  } else if (!SD.begin(chipSelect)) {
    #if DEBUG
      Serial.println("Error: Failed to initialize the SD library with the SD.h interface.");
    #endif

    return 0;
  }

  #if DEBUG
    Serial.println("Finished setting up SD card (wiring is correct and card is present).");
  #endif

  return 1;
}

/**
 *  Prints SD card info.
 *
 *  @returns 1 if successful, and 0 otherwise
 */
int cardInfo() {
  //  Print card type
  Serial.print("\nCard type:\t\t");

  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
      break;
  }

  //  Attempt to open the volume or partition

  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card.");

    return 0;
  }

  Serial.print("Clusters:\t\t");
  Serial.println(volume.clusterCount());
  Serial.print("Blocks per Cluster:\t");
  Serial.println(volume.blocksPerCluster());
  Serial.print("Total Blocks:\t\t");
  Serial.println(volume.blocksPerCluster() * volume.clusterCount());

  Serial.println();

  //  print the type and size of the first FAT-type volume
  uint32_t volumeSize;

  Serial.print("Volume type is:\t\tFAT");
  Serial.println(volume.fatType(), DEC);

  volumeSize = volume.blocksPerCluster();  //  clusters are collections of blocks
  volumeSize *= volume.clusterCount();
  volumeSize /= 2;  //  SD card blocks are always 512 bytes (2 blocks are 1 KB)

  Serial.print("Volume size (KB):\t");
  Serial.println(volumeSize);

  Serial.print("Volume size (MB):\t");
  volumeSize /= 1024;

  Serial.println(volumeSize);

  Serial.print("Volume size (GB):\t");
  Serial.println((float)volumeSize / 1024.0);

  Serial.println("\nFiles found on the card (name, date, and size in bytes): ");

  root.openRoot(&volume);

  //  list all files in the card with date and size

  root.ls(LS_R | LS_DATE | LS_SIZE);

  root.close();

  return 1;
}

/**
 *  Opens ByteFrost disk file (if not yet open).
 *
 */
// void openDiskFile() {
//   disk = SD.open("disk");
// }

/**
 *  Prints contents of the given file to the Serial Monitor, if it exists.
 *
 *  @param filename Name of the file to print to the Serial Monitor
 *  @returns 1 if successful, and 0 otherwise
 */
int printFile(String filename) {
  Serial.print(filename);
  if (SD.exists(filename)) {
    Serial.println(" exists.");
  } else {
    Serial.println(" doesn't exist.");
    return 0;
  }

  Serial.println("Opening " + filename + "...");

  File file = SD.open(filename);

  int bytesRead;
  uint8_t buf[1];
  int maxColLen = 128;
  int colLen = 0;
  while (file.available()) {
    bytesRead = file.read(buf, 1);

    Serial.write(buf[0]);

    if (buf[0] == '\n') {
      colLen = 0;
    } else {
      colLen++;

      if (colLen == maxColLen) {
        Serial.println();
        colLen = 0;
      }
    }
  }

  Serial.println("\nFinished reading " + filename + ".");

  file.close();

  return 1;
}

  uint16_t interruptHappenedFlag; 


void setup() {
  // put your setup code here, to run once:

  interruptHappenedFlag = 0; // Be ready for interupt.
  //  Setup pins
  //  Data bus (PORTD) will be used as input / output dependng on the received command

  //  Default values
  digitalWrite(CNT, LOW);
  digitalWrite(RST, HIGH);
  digitalWrite(SHIFT, LOW);
  digitalWrite(BUS_RELEASE, HIGH);  // Always keep the bus

  pinMode(GRANT_INT, INPUT);
  pinMode(CNT, OUTPUT);
  pinMode(RST, OUTPUT);
  pinMode(SERIAL_IN, INPUT);
  pinMode(SHIFT, OUTPUT);  
  pinMode(BUS_RELEASE, OUTPUT);

  //  Disable Memory Read and Write = //digitalWrite(WRITE_INV, HIGH);  //digitalWrite(SRAM_OE_INV, HIGH);
  REG_PORT_OUTSET1 = WE_INV_PB02 | SRAM_OE_INV_PB03;
  // Make READ_INV and WRITE_INV output pins =  //pinMode(WRITE_INV, OUTPUT);  //pinMode(SRAM_OE_INV, OUTPUT);
  REG_PORT_DIRSET1 = WE_INV_PB02 | SRAM_OE_INV_PB03;

  //  Attach GO interrupt
  attachInterrupt(digitalPinToInterrupt(GRANT_INT), BusGrant, RISING);

  #if DEBUG
    Serial.begin(500000);

    //  There is a delay between the call to Serial.begin() and the serial stream
    //  opening; this busy wait ensures that the following Serial printouts are
    //  printed
    //  From https://forum.arduino.cc/t/cant-view-serial-print-from-setup/167916/2
    while (!Serial)
      ;

    Serial.println("MKRZero SD Card Driver");
  #endif

  initialize();
}

/**************************************************
/*  Memory Utility Functions:                     *
/*  -------------------------                     *
/*    o    setBusDirection                        *
/*    o    writeToBus                             *
/*    o    readFromBus                            *
/*    o    readSector2Buffer                      *
/*    o    writeBuffer2Sector                     *
/*    o    readMem2Buffer                         *
/*    o    writeBuffer2Mem                        *
/**************************************************/

void setBusDirection(int mode) 
{
  if(mode == OUTPUT)
  {
      REG_PORT_DIRSET0 = 0x00F00C00;
      REG_PORT_DIRSET1 = 0x00000C00;
  }
  else
  {
      REG_PORT_DIRCLR0 = 0x00F00C00;
      REG_PORT_DIRCLR1 = 0x00000C00;
  }
}

void writeToBus(uint8_t currentByte)
{
  // Serial.print("writeToBus: ");
  // Serial.println(data);

      REG_PORT_OUT0 = (REG_PORT_OUT0 & 0xFF0FF3FF) | ((currentByte & 0x03) << 22) |  // PA23:PA22 - 1:0
                      ((currentByte & 0x0C) << 8) |                                  // PA11:PA10 - 3:2
                      ((currentByte & 0xC0) << 14);                                  // PA21:PA20 - 7:6
      REG_PORT_OUT1 = (REG_PORT_OUT1 & 0xFFFFF3FF) | ((currentByte & 0x30) << 6);    // PB11:PB10 - 5:4
}

uint8_t readFromBus() {
  // Serial.println("readFromBus()");

  //  Read from bus

  uint32_t port0 = REG_PORT_IN0;
  uint32_t port1 = REG_PORT_IN1;

  // data =   (port0 & 0x00F00C00);
  uint32_t bits10 = (port0 & 0x00C00000) >> 22;
  uint32_t bits32 = (port0 & 0x00000C00) >> 8;
  uint32_t bits54 = (port1 & 0x00000C00) >> 6;
  uint32_t bits76 = (port0 & 0x00300000) >> 14;

  return bits76 | bits54 | bits32 | bits10;
}

/**
 * readSector2Buffer() Read sector sectorId into sectorDataBuffer
 */
uint16_t readSector2Buffer(uint16_t sectorID)
{
  #if DEBUG
    Serial.print("Reading sector ");
    Serial.print(sectorID);
    Serial.println(" from disk...");
  #endif

  //  Open ByteFrost disk file. (Assuming the disk file is not currently open (based on
  //  the SD card library's documentation for SD.open(), only one file may be open at
  //  one time)).
  if (!SD.exists("disk")) {
    #if DEBUG
      Serial.println("Error! No file 'disk' exists on the SD card!");
    #endif

    //  Stop working (ByteFrost will be stuck since the SD card driver won't send
    //  a bus release signal to the bus arbiter)
    while (1)
      ;
  }

  File disk = SD.open("disk");

  //  Move to the correct sector. (Assuming an 11-bit sector ID!)
  if (!disk.seek(sectorID * SECTOR_SIZE)) {
    #if DEBUG
      Serial.print("Error! Couldn't seek disk file to sector ");
      Serial.print(sectorID);
      Serial.println("!");
    #endif

    //  Stop working (ByteFrost will be stuck since the SD card driver won't send
    //  a bus release signal to the bus arbiter)
    while (1)
      ;
  }

  //  Copy sector contents to the sector buffer
  disk.read(sectorDataBuffer, SECTOR_SIZE);

  #if DEBUG
    Serial.println("Sector data:\n");
    for (int i = 0; i < SECTOR_SIZE / 8; i++) {
      for (int j = 0; j < 8; j++) {
        Serial.print(sectorDataBuffer[8*i + j]);
      }
      Serial.println();
    }
  #endif

  //  Close disk file
  disk.close();
}
/**
 * writeBuffer2Sector() Write sectorDataBuffer into sector sectorId  
 */
uint16_t writeBuffer2Sector(uint16_t sectorID)
{
  #if DEBUG
    Serial.print("Writing to sector ");
    Serial.print(sectorID);
    Serial.println("...");
  #endif

  //  Open ByteFrost disk file. (Assuming the disk file is not currently open (based on
  //  the SD card library's documentation for SD.open(), only one file may be open at
  //  one time)).
  if (!SD.exists("disk")) {
    #if DEBUG
      Serial.println("Error! No file 'disk' exists on the SD card!");
    #endif

    //  Stop working (ByteFrost will be stuck since the SD card driver won't send
    //  a bus release signal to the bus arbiter)
    while (1)
      ;
  }

  File disk = SD.open("disk", O_WRITE);

  //  Move to the correct sector. (Assuming an 11-bit sector ID!)
  if (!disk.seek(sectorID * SECTOR_SIZE)) {
    #if DEBUG
      Serial.print("Error! Couldn't seek disk file to sector ");
      Serial.print(sectorID);
      Serial.println("!");
    #endif

    //  Stop working (ByteFrost will be stuck since the SD card driver won't send
    //  a bus release signal to the bus arbiter)
    while (1)
      ;
  }

  //  Copy buffer contents to sector
  disk.write(sectorDataBuffer, SECTOR_SIZE);

  //  Close disk file
  disk.close();
}
/**
 * readMem2Buffer() Read memory from address into sectorDataBuffer
 */
uint16_t readMem2Buffer()
{
    uint16_t i;
    //  Prepare for read, Enable SRAM Output   
    REG_PORT_OUTCLR1 = SRAM_OE_INV_PB03;
 
    //  Set PORTD to be incoming
    // DDRD = B00000000; //  All 8 bits are incoming
    setBusDirection(INPUT);

    //  Send reset to counter address
    digitalWrite(RST, LOW);
    delayMicroseconds(1);
    digitalWrite(RST, HIGH);
    for ( i = 0; i < SECTOR_SIZE; i++) 
    {
      sectorDataBuffer[i] = readFromBus();

      #if DEBUG
        Serial.print("Reading ");
        Serial.print(sectorDataBuffer[i]);
        Serial.println(" from bus.");
        delay(50);
      #endif

      //  Increment address counter (rising edge)
      REG_PORT_OUTSET0 = CNT_PA17;
      REG_PORT_OUTCLR0 = CNT_PA17;
    }

    //  Finish memory read, Disable SRAM Output   
    REG_PORT_OUTSET1 = SRAM_OE_INV_PB03;
    return i;
}
/**
 * writeBuffer2Mem() Write sectorDataBuffer into memory starting with adress.  
 */
uint16_t writeBuffer2Mem()
{
    uint16_t i;
    //  Disable Read and Write
    REG_PORT_OUTSET1 = WE_INV_PB02 | SRAM_OE_INV_PB03;

    //  Send reset to reset counter address to 0 (i.e., 9 LSBs are 0)
    REG_PORT_OUTCLR0 = RST_PA19 | CNT_PA17;

    //  Finished reset of counters (74LS161 chips)
    REG_PORT_OUTSET0 = RST_PA19;

    setBusDirection(OUTPUT);
   
    for (i = 0; i < SECTOR_SIZE; i++) 
    {
      //  Set data to next value

      writeToBus(sectorDataBuffer[i]);
      //  Set write enable
      REG_PORT_OUTCLR1 = WE_INV_PB02;
      //  Finish write cycle (25 ns is enuogh)
      delayMicroseconds(1);
      #if DEBUG
        Serial.print("Writing ");
        Serial.print(sectorDataBuffer[i]);
        Serial.println(" to bus.");
        delay(50);
      #endif
      REG_PORT_OUTSET1 = WE_INV_PB02;

      //  Increment address counter (rising edge)
      REG_PORT_OUTSET0 = CNT_PA17;
      REG_PORT_OUTCLR0 = CNT_PA17;

    }
    setBusDirection(INPUT);
    //  Finish SRAMread, Disable SRAM Read and Write
    REG_PORT_OUTSET1 = WE_INV_PB02 | SRAM_OE_INV_PB03;

    return i;
}

/**
 * Debug - To remove
 * writeCommandToSR() Write hiByte and loByte to Shift Register.  
 * This is a debug function only
 * It requires 
 *     LoByte to end with 010
 *     hiByte to end with 001
 *
 */
void writeCommandToSR(uint8_t loByte, uint8_t hiByte)
{
    Serial.print("SR loByte: 0x");
    Serial.println(loByte, HEX);
    Serial.print("SR hiByte: 0x");
    Serial.println(hiByte, HEX);
    //  Disable SRAM Read, Enable Write (for debug only)
    REG_PORT_OUTSET1 = SRAM_OE_INV_PB03;
    REG_PORT_OUTCLR1 = WE_INV_PB02;

    setBusDirection(OUTPUT);
 
    writeToBus(loByte);
    // Debug only use RESET to trigger Load
    REG_PORT_OUTCLR0 = RST_PA19;
    REG_PORT_OUTSET0 = RST_PA19;

    writeToBus(hiByte);
    REG_PORT_OUTCLR0 = RST_PA19;
    REG_PORT_OUTSET0 = RST_PA19;

    setBusDirection(INPUT);
    REG_PORT_OUTSET1 = WE_INV_PB02 | SRAM_OE_INV_PB03;
}
/**
 * readCommandFromSR() 
 *
 */
uint16_t readCommandFromSR(void)
{
    uint16_t sr_cmd;
    uint8_t sr_bit;
    sr_cmd = 0;
    for (int i = 0; i<16; i++)
    {
        sr_bit = digitalRead(SERIAL_IN);
        sr_cmd  = (sr_cmd << 1) | sr_bit;
        digitalWrite(SHIFT, HIGH);
        digitalWrite(SHIFT, LOW);
        #if DEBUG
          Serial.print("SR bit: ");
          Serial.println(sr_bit);
        #endif DEBUG
    }
    // Serial.print("SR Command: 0x");
    // Serial.println(sr_cmd, HEX);
    return sr_cmd;
}
void loop() 
{
   if(interruptHappenedFlag)
   {
        uint16_t sr_cmd;
        //  Received go interrupt
        #if DEBUG
          Serial.println("Got a Bus Grant Interrupt");
        #endif
        sr_cmd = readCommandFromSR();
        #if DEBUG
          Serial.print("SR Command: 0x");
          Serial.println(sr_cmd, HEX);
          // delay(2000);
          // Serial.println("Release the bus");
        #endif
        
                
        //  Handle the command
        //  Disk Command Format:  16 bits-long command
        //  15  14  13  12  11  10 - 0
        //  R/W ?   ?   ?   ?   Sector pointer / ID

        uint16_t sectorID = sr_cmd & (0x07FF);
        bool writeToDisk = (sr_cmd & (0x8000)) == 0x8000;

        #if DEBUG
          Serial.println("SR Command ======");
          Serial.print("Writing to Disk? ");
          Serial.println(writeToDisk);
          Serial.print("Sector ID: ");
          Serial.println(sectorID);
        #endif

        if (writeToDisk) {
          //  Write page (256 bytes) to a sector (256 bytes) in disk
          readMem2Buffer();

          writeBuffer2Sector(sectorID);
        }
        else {
          //  Read a sector (256 bytes) from disk to a page (256 bytes) in RAM
          readSector2Buffer(sectorID);

          writeBuffer2Mem();
        }

        // Release the flag for another interrupt  
        interruptHappenedFlag = 0; // Be ready for interupt.
        digitalWrite(BUS_RELEASE, LOW);  // Release the bus
        while(digitalRead(GRANT_INT))
        {
          delayMicroseconds(10);  // Wait for the bus get released
        }; 
        digitalWrite(BUS_RELEASE, HIGH); // Get ready for the next command
   }
}

/**
 *  BusGrant interrupt handler.
 */
void BusGrant() 
{
    interruptHappenedFlag = 1; // Flag interrupt.
}







