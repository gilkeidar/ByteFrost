#   Disk

Includes code to access the ByteFrost's disk (microSD card) from another computer and the Arduino code that provides the ByteFrost with a simple read sector / write sector interface to this disk.

##  Disk Design

The ByteFrost disk works as follows:

The microSD card used as the disk contains a FAT32 filesystem with at least one file called "disk".
Since the filesystem here is FAT32, we can easily access the SD card's contents in a conventional OS,
and can use a program utility to alter the ByteFrost's disk contents just by reading/writing to this file
(for simplicity, perhaps include this utility on the SD card so that it doesn't have to search for the
"disk" file?)
The Arduino card (MKRZero) will meanwhile use the SD card library to open this file, and provide the
ByteFrost a simple readSector / writeSector interface.
The ByteFrost only sees this interface and so can ignore the SD card's filesystem. This means that the
ByteFrost disk can implement any or no filesystem, which can allow us to simplify the ByteFrost's
filesystem as the ByteFrost's kernel will need to handle all filesystem operations.

## Arduino IDE Setting
Board:
Select Tools -> Board -> Arduino SAMD Boards -> Arduino MKR Zero
Note: if the wrong board is selected, compilation will fail cacause it would not find the sam.h file