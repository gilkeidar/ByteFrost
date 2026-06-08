/*  File-related data   */

#define DEFAULT_DISK_FILE_NAME  "disk"
#define MAX_FILE_NAME_LEN       1024

//  ByteFrost FileSystem version 1
#define DISK_SIZE_KB            512
#define BLOCK_SIZE              256                 //  bytes
#define FILES_DIR_NAME          "files"
#define BOOT_FILE_NAME          "boot.bin"
#define MAX_NUM_FILES           16
#define FILE_NAME_CHAR_LIMIT    15
#define MAX_FILE_SIZE           (31 * 1024) + 512   //  31.5 KB
#define MAX_BOOT_FILE_SIZE      2 * 1024            //  2 KB
#define DIRECTORY_ENTRY_SIZE    16                  //  16 bytes

//  ByteFrost FileSystem version 1 layout
#define DIRECTORY_BLOCK         8
#define DATA_FREE_MAP_BLOCK     9
#define DATA_FREE_MAP_LEN       252                 //  Number of bytes used
#define INODE_FREE_MAP_BLOCK    10
#define INODE_FREE_MAP_LEN      2                   //  Number of bytes used
#define FIRST_INODE_BLOCK       11
#define LAST_INODE_BLOCK        26
#define FIRST_DATA_BLOCK        27
#define LAST_DATA_BLOCK         2042

//  Inode structure
#define FILE_SIZE_BYTES_OFFSET  0
#define FILE_SIZE_BLOCKS_OFFSET 2
#define FILE_TYPE_OFFSET        3
#define FIRST_DATA_PTR_OFFSET   4

typedef enum FileType {
    UNKNOWN,
    TEXT,
    ASM,
    MLG,
    BIN
} FileType;

typedef struct DiskDirectory 
{
    FILE * files[MAX_NUM_FILES];        //  array of FILE pointers in files/ dir
    char * file_names[MAX_NUM_FILES];   //  array of file names
    FileType types[MAX_NUM_FILES];      //  array of file types
    int file_sizes[MAX_NUM_FILES];      //  array of file sizes (bytes)
    int num_files;                      //  Number of files in files/ dir              
    FILE * boot_file;                   //  Boot file FILE pointer
} DiskDirectory;