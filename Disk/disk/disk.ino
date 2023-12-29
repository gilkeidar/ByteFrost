#include <SD.h>

#define OVERWRITE_DISK  0
#define DEBUG           1
#define DISK_BLOCK_SIZE 256

//  PIN definitions
//  Data Bus (RAM read / write data) PORTD (digital pins 0 - 7)
#define GO_INT      8   //  Go interrupt (incoming) on digital pin 8
#define CNT         9   //  Counter count signal (outgoing) on digital pin 9
#define RST         10  //  Counter reset signal (outgoing) on digital pin 10
#define SERIAL_IN   11  //  Serial data (incoming) on digital pin 11
#define SHIFT       12  //  Shift register shift signal (outgoing) on digital pin 12

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
  pinMode(GO_INT, INPUT);
  pinMode(CNT, OUTPUT);
  pinMode(RST, OUTPUT);
  pinMode(SERIAL_IN, INPUT);
  pinMode(SHIFT, OUTPUT);

  //  Attach GO interrupt
  attachInterrupt(digitalPinToInterrupt(GO_INT), go, RISING);

  #if DEBUG
  Serial.begin(9600);

  //  There is a delay between the call to Serial.begin() and the serial stream
  //  opening; this busy wait ensures that the following Serial printouts are
  //  printed
  //  From https://forum.arduino.cc/t/cant-view-serial-print-from-setup/167916/2
  while (!Serial)
    ;

  Serial.println("MKRZero SD Card Driver");
  #endif

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
    && (config = SD.open(BYTEFROST_CONFIG_FILE_NAME.c_str(), FILE_READ))) 
  {
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
  }
  else {
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
  Serial.println("Printing the first 30 disk blocks...\n");

  char buf[DISK_BLOCK_SIZE];
  int bytesRead;
  for (int i = 0; i < 200; i++) {
    bytesRead = disk.read(buf, DISK_BLOCK_SIZE);

    if (bytesRead != DISK_BLOCK_SIZE) {
      Serial.print("Error: Failed to read disk block ");
      Serial.print(i);
      Serial.print(": Tried to read ");
      Serial.print(DISK_BLOCK_SIZE);
      Serial.print(" bytes, but read ");
      Serial.print(bytesRead);
      Serial.println(" bytes instead.");

      while (1)
        ;
    }
    // Serial.print("Block ");
    // Serial.print(i);
    // Serial.println(":");

    Serial.write(buf);
    
    // for (int j = 0; j < DISK_BLOCK_SIZE; j++) {
    //   Serial.write(buf[j]);

    //   // if (j % 64 == 0)
    //   //   Serial.write("\n");
    // }

    // Serial.println();
  }

  #endif

}

void loop() {
  // put your main code here, to run repeatedly:
  // Serial.println("Hello World from MKRZero!");
}
