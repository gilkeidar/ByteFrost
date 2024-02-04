#include <SD.h>
#include <sam.h>

#define OVERWRITE_DISK 0
#define DEBUG 1
#define SECTOR_SIZE 256

// Global buffer that serves the four operations:
//        SDRead:     SD -> Buffer
//        SDWrite:    Buffer -> SD
//        memoryRead: Memory -> Buffer
//        memoryWrite: Buffer -> Memory
char sectorDataBuffer[SECTOR_SIZE] = { 0 };



//  PIN definitions
//  Data Bus (RAM read / write data) PORTD (digital pins 0 - 7)
#define GO_INT     8  //  Go interrupt (incoming) on digital pin 8 (A1)
#define CNT        9  //  Counter count signal (outgoing) on digital pin 9 (A2)
#define RST       10  //  Counter reset signal (outgoing) on digital pin 10 (A3)
#define SERIAL_IN 11  //  Serial data (incoming) on digital pin 11 (A4)
#define SHIFT     12  //  Shift register shift signal (outgoing) on digital pin 12 (A5)

// #define WRITE_INV       16  //  SRAM Read / Write (outgoing) on digital pin 13 (A6)
//                             //  When WRITE_INV = 0, then data is written to SRAM; when it is 1, then data is read from SRAM
// #define SRAM_OE_INV     17  //  SRAM Output Enable (outgoing) on digital pin 14 (A7) (INVERSE, active low)
//                             //  When SRAM_OE_INV = 0, then SRAM outputs data; when it is 1, SRAM data output is HIGH Z

uint RST_PA19 = 1 << 19;
uint CNT_PA17 = 1 << 17;
uint WE_INV_PB02 = 1 << 2;
uint SRAM_OE_INV_PB03 = 1 << 3;

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

/**
 *  Initializes SD Card (using SD.h interface and direct Sd2Card interface).
 *
 *  @returns 1 if successful, and 0 otherwise
 */
int initialize() {
  //  Setup SD Card
  Serial.println("Setting up SD card...");

  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("Error: Failed to initialize the SD library and card.");

    return 0;
  } else if (!SD.begin(chipSelect)) {
    Serial.println("Error: Failed to initialize the SD library with the SD.h interface.");

    return 0;
  }

  Serial.println("Finished setting up SD card (wiring is correct and card is present).");

  return 1;
}

/**
 *  Close the SD card.
 */
int closeCard() {
  SD.end();
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

/**
 *  Creates a ByteFrost disk file on this SD card. If one already exists, this does nothing.
 *
 *  @param truncate if true (non-zero), override disk if it exists.
 *  @returns 1 if successful, and 0 otherwise.
 */
// int createDisk(int truncate) {
//   // Serial.println("Checking whether this SD card already has a ByteFrost disk file...");

//   if (!truncate && SD.exists(BYTEFROST_DISK_NAME)) {
//     Serial.println("This SD card already has a ByteFrost disk file. Returning.");

//     return 1;
//   }

//   Serial.println("No ByteFrost disk file found or truncate was set. Creating one...");

//   File disk;
//   if (!(disk = SD.open(BYTEFROST_DISK_NAME, O_WRITE | O_CREAT))) {
//     Serial.println("Error: Failed to create ByteFrost disk file \"" + BYTEFROST_DISK_NAME + "\"!");
//     return 0;
//   }

//   Serial.print("Initializing disk size to ");
//   Serial.print(BYTEFROST_DISK_SIZE, DEC);
//   Serial.println(" KB.");

//   //  Write from start of file
//   if (!disk.seek(0)) {
//     Serial.println("Error: Couldn't move to start of file.");
//   }

//   //  Allocate a buffer
//   uint8_t buffer[1024];

//   //  Write buffer to disk file to set its size correctly

//   for (int i = 0; i < BYTEFROST_DISK_SIZE; i++) {
//     uint32_t bytesWritten = disk.write(buffer, 1024);

//     if (bytesWritten != 1024) {
//       Serial.print("Error: disk file initialized incorrectly.");

//       Serial.println("Attempting to remove corrupted disk file...");

//       disk.close();

//       if (!SD.remove(BYTEFROST_DISK_NAME)) {
//         Serial.println("Error: Couldn't delete corrupted disk file.");

//         return 0;
//       }

//       return 0;
//     }
//   }

//   disk.close();

//   Serial.print("Successfully created file \"" + BYTEFROST_DISK_NAME + " \" of size ");
//   Serial.print(BYTEFROST_DISK_SIZE, DEC);
//   Serial.println(" KB.");

//   return 1;
// }

/**
 *  Go interrupt handler.
 */
void go() {
//  Received go interrupt
#if DEBUG
  Serial.println("Go interupt handler.");
#endif
}

void setup() {
  // put your setup code here, to run once:

  //  Setup pins
  //  Data bus (PORTD) will be used as input / output dependng on the  received command

  //  Default values
  digitalWrite(CNT, LOW);
  digitalWrite(RST, HIGH);
  digitalWrite(SHIFT, LOW);
 
  
  pinMode(GO_INT, INPUT);
  pinMode(CNT, OUTPUT);
  pinMode(RST, OUTPUT);
  pinMode(SERIAL_IN, INPUT);
  pinMode(SHIFT, OUTPUT);
  
  //  Disable Memory Read and Write = //digitalWrite(WRITE_INV, HIGH);  //digitalWrite(SRAM_OE_INV, HIGH);
  REG_PORT_OUTSET1 = WE_INV_PB02 | SRAM_OE_INV_PB03;
  // Make READ_INV and WRITE_INV output pins =  //pinMode(WRITE_INV, OUTPUT);  //pinMode(SRAM_OE_INV, OUTPUT);
  REG_PORT_DIRSET1 = WE_INV_PB02 | SRAM_OE_INV_PB03;

  //  Attach GO interrupt
  attachInterrupt(digitalPinToInterrupt(GO_INT), go, RISING);


  // Serial.begin(9600);
  Serial.begin(500000);

  //  There is a delay between the call to Serial.begin() and the serial stream
  //  opening; this busy wait ensures that the following Serial printouts are
  //  printed
  //  From https://forum.arduino.cc/t/cant-view-serial-print-from-setup/167916/2
  while (!Serial)
    ;

  Serial.println("MKRZero SD Card Driver");


  //  Initialize SD card
  if (!initialize()) {
    while (1)
      ;
  }

#if DEBUG
  Serial.println("================================");

  if (!cardInfo()) {
    while (1)
      ;
  }

  Serial.println("================================");

  //  Print README.txt
  if (!printFile("readme.txt")) {
    while (1)
      ;
  }

  Serial.println("================================");

  //  Create ByteFrost disk file (or if it exists, do nothing)
  // if (!createDisk(OVERWRITE_DISK)) {
  //   while (1)
  //     ;
  // }

#endif

  //  Disk file name to search for (get from bytefrost.config)
  String disk_file_name = BYTEFROST_DEFAULT_DISK_NAME;

  //  Check that a config file exists and if so try to open it
  if (SD.exists(BYTEFROST_CONFIG_FILE_NAME)
      && (config = SD.open(BYTEFROST_CONFIG_FILE_NAME.c_str(), FILE_READ))) {
//  Successfully opened the config file
#if DEBUG
    Serial.println("Found " + BYTEFROST_CONFIG_FILE_NAME + " file!");
#endif

    //  Store disk name in buffer
    int disk_file_name_length = config.available();

    if (disk_file_name_length > MAX_DISK_NAME_LEN) {
#if DEBUG
      Serial.print("Error: disk name length is over ");
      Serial.print(MAX_DISK_NAME_LEN);
      Serial.println(" chars.");
#endif
      while (1)
        ;
    }

    char disk_name[disk_file_name_length + 1];

    config.read(disk_name, disk_file_name_length);

    disk_name[disk_file_name_length] = '\0';

#if DEBUG
    Serial.print("Disk file name: ");
    Serial.println(disk_name);
#endif

    disk_file_name = disk_name;
  } else {
//  Create the config file with default disk name
#if DEBUG
    Serial.println("Error: " + BYTEFROST_CONFIG_FILE_NAME + " file not found. Creating it with default disk name.");
#endif

    if (!(config = SD.open(BYTEFROST_CONFIG_FILE_NAME.c_str(), FILE_WRITE))) {
//  Failed to create config file
#if DEBUG
      Serial.println("Error: Couldn't create " + BYTEFROST_CONFIG_FILE_NAME + " file.");
#endif

      while (1)
        ;
    }

    //  Write disk name into config file
    int bytesWritten = config.print(disk_file_name);

    if (bytesWritten != disk_file_name.length()) {
#if DEBUG
      Serial.print("Error: Tried to write ");
      Serial.print(disk_file_name.length());
      Serial.print(" bytes to " + BYTEFROST_CONFIG_FILE_NAME + ", but only wrote ");
      Serial.print(bytesWritten);
      Serial.println(" bytes");
#endif

      while (1)
        ;
    }

    config.close();

#if DEBUG
    Serial.println("Created " + BYTEFROST_CONFIG_FILE_NAME + " file.");
#endif
  }

//  Open ByteFrost disk
#if DEBUG
  Serial.println("Opening ByteFrost disk file '" + disk_file_name + "'...");
#endif

  if (!(disk = SD.open(disk_file_name))) {
#if DEBUG
    Serial.println("Error: Couldn't open disk file.");
#endif
    while (1)
      ;
  }

#if DEBUG
  Serial.println("Opened ByteFrost disk file '" + disk_file_name + "'.");
#endif

#if DEBUG
  Serial.println("Printing the first SECTOR_SIZE disk blocks...\n");

  int bytesRead;
  for (int i = 0; i < SECTOR_SIZE; i++) {
    bytesRead = disk.read(sectorDataBuffer, SECTOR_SIZE);

    if (bytesRead != SECTOR_SIZE) {
      Serial.print("Error: Failed to read disk block ");
      Serial.print(i);
      Serial.print(": Tried to read ");
      Serial.print(SECTOR_SIZE);
      Serial.print(" bytes, but read ");
      Serial.print(bytesRead);
      Serial.println(" bytes instead.");

      while (1)
        ;
    }
    // Serial.print("Block ");
    // Serial.print(i);
    // Serial.println(":");

    Serial.write(sectorDataBuffer);

    // for (int j = 0; j < SECTOR_SIZE; j++) {
    //   Serial.write(buf[j]);

    //   // if (j % 64 == 0)
    //   //   Serial.write("\n");
    // }

    // Serial.println();
  }

  Serial.flush();

#endif
}

/**************************************************
/*  Memory Utility Functions:                     *
/*  -------------------------                     *
/*    o    setBusDirection                        *
/*    o    writeToBus                             *
/*    o    readFromBus                            *
/*    o    readSector2Beffer                      *
/*    o    writeBeffer2Sector                     *
/*    o    readMem2Buffer                         *
/*    o    writeBeffer2Mem                        *
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
 * readSector2Beffer() Read sector sectorId into sectorDataBuffer
 */
uint16_t readSector2Beffer(uint16_t sectorId)
{

}
/**
 * writeBeffer2Sector() Write sectorDataBuffer into sector sectorId  
 */
uint16_t writeBeffer2Sector(uint16_t sectorId)
{

}
/**
 * readMem2Buffer() Read memory from address into sectorDataBuffer
 */
uint16_t readMem2Buffer(uint16_t address)
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
      //  Increment address counter (rising edge)
      REG_PORT_OUTSET0 = CNT_PA17;
      REG_PORT_OUTCLR0 = CNT_PA17;
    }

    //  Finish memory read, Disable SRAM Output   
    REG_PORT_OUTSET1 = SRAM_OE_INV_PB03;
    return i;
 
}
/**
 * writeBeffer2Mem() Write sectorDataBuffer into memory starting with adress.  
 */
uint16_t writeBeffer2Mem(uint16_t address)
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

int num_iterations = 0;
void loop() {
  // put your main code here, to run repeatedly:
  // Serial.println("Hello World from MKRZero!");
  // digitalWrite(SHIFT, HIGH);
  // REG_PORT_OUTSET1 = WE_INV_PB02 | SRAM_OE_INV_PB03;
  // delay(1000);
  // REG_PORT_OUTCLR1 = WE_INV_PB02 | SRAM_OE_INV_PB03;
  // delay(1000);

  //  Loop to write a sector then read a sector once (then end)
  
  uint16_t return_val;

  if (num_iterations < 1) {

    for(int i=0; i< SECTOR_SIZE; i++) 
        sectorDataBuffer[i] = i; 

    return_val = writeBeffer2Mem(0);

    Serial.print("writeBeffer2Mem: ");
    Serial.println(return_val);
   
    memset(sectorDataBuffer, 0, sizeof(sectorDataBuffer)); 

    return_val = readMem2Buffer(0);

    Serial.print("readMem2Buffer: ");
    Serial.println(return_val);

// Now check the read value is equal to the one initially written.
    uint good_results = 0;
    //  Read SECTOR_SIZE bytes

    for (uint16_t i = 0; i < SECTOR_SIZE; i++) {
      //  Get current pointed to data value
     
      if (sectorDataBuffer[i] != i)
      {
        Serial.print("value at address ");
        Serial.print(i);
        Serial.print(":\t\t");
        Serial.print((uint8_t)sectorDataBuffer[i]);
        Serial.print(" | Broken bits: ");
        Serial.print(sectorDataBuffer[i] ^ i);
        Serial.print(" | Difference: ");
        Serial.println(sectorDataBuffer[i] - i);
      } else 
      {
        good_results++;
      }
    }

    // digitalWrite(SRAM_OE_INV, HIGH);
    Serial.print("Good values: ");
    Serial.print(good_results);
    Serial.print(" / ");
    Serial.println(SECTOR_SIZE);
    num_iterations++;
  }
}
