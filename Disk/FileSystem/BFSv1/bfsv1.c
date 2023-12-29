/**
 * @file bfsv1.c
 * @author Gil Keidar
 * @brief ByteFrost FileSystem version 1 command-line utility
 * @version 0.1
 * @date 2023-12-26
 */

#define FALSE   0
#define TRUE    1

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include "error.h"
#include "files.h"

//  Flags
int p_flag = FALSE;             //  -p flag (pack directory into disk file)
int u_flag = FALSE;             //  -u flag (unpack disk file into directory)
int o_flag = FALSE;             //  -o flag (specifies output disk file name)

//  Arguments
char * disk_file_name = NULL;   //  Specified disk file name
char * disk_dir_name = NULL;    //  Specified disk directory name

//  Function prototypes

/**
 * @brief Parse utility arguments from commandline
 * 
 * @param argv Argument array passed into main()
 * @param argc Length of argument array passed into main()
 */
void getInput(char ** argv, int argc);
void pack();
void unpack();
FILE * createBFSDiskv1(char * disk_name);
DiskDirectory * getDiskDirectory(char * directory_name);
void writeBootFile(DiskDirectory * disk_dir, FILE * disk);
void writeFileToDisk(DiskDirectory * disk_dir, int fileIndex, FILE * disk);
int allocateInode(FILE * disk);
int allocateDataBlock(FILE * disk);
void allocateDirectoryEntry(char * filename, int inode_block, FILE * disk);
void setInodeMetadata(FileType type, int inode_block, FILE * disk);

FileType getFileType(char * extension);

char ** splitFileExtension(char * filename);

int logicalShiftRight(int x, int shiftAmt);

int main(int argc, char ** argv) {
    /*  ByteFrost FileSystem ver. 1 Command-line utility    */

    //  Get input
    getInput(argv, argc);

    // printf("p_flag: %d\tu_flag: %d\to_flag: %d\tdisk file name: %s", p_flag, 
    //     u_flag, o_flag, (disk_file_name == NULL ? "NULL" : disk_file_name));

    // printf("\tdisk dir name: %s\n", 
    //     (disk_dir_name == NULL ? "NULL" : disk_dir_name));

    if (p_flag) {
        pack();
    }
    else if (u_flag) {
        unpack();
    }
    else {
        fprintf(stderr, "Error: Neither -p nor -u specified.\n");
        exit(UNEXPECTED_ERROR);
    }
}


void getInput(char ** argv, int argc) {
    printf("getInput()\n");

    int index;
    int looking_for_param = FALSE;  //  Looking for parameter
    char current_flag;              //  current flag (for param)

    for (index = 1; index < argc; index++) {
        if (!looking_for_param && argv[index][0] == '-') 
        {
            if (strlen(argv[index]) < 2) {
                //  String is "-" (invalid flag)
                fprintf(stderr, "Error: Missing flag after '-'.\n");
                exit(INCORRECT_USAGE);
            }

            //  argv[index] contains at least 2 characters (safe to access
            //  argv[index][1])
            switch (argv[index][1]) 
            {
                case 'u':
                    u_flag = TRUE;
                    looking_for_param = TRUE;
                    current_flag = 'u';
                    break;
                case 'p':
                    p_flag = TRUE;
                    looking_for_param = TRUE;
                    current_flag = 'p';
                    break;
                case 'o':
                    o_flag = TRUE;
                    looking_for_param = TRUE;
                    current_flag = 'o';
                    break;
                default:
                    fprintf(stderr, "Error: Unknown flag used.\n");
                    exit(UNKNOWN_FLAG);
                    break;
            }
        }
        else if (looking_for_param) {
            if (argv[index][0] == '-') {
                fprintf(stderr, "Error: Flag cannot be a parameter value.\n");
                exit(INCORRECT_USAGE);
            }

            switch (current_flag) {
                case 'o':
                case 'u':
                    disk_file_name = argv[index];
                    break;
                case 'p':
                    disk_dir_name = argv[index];
                    //  Remove trailing forward slash if included
                    if (argv[index][strlen(argv[index]) - 1] == '/')
                        disk_dir_name[strlen(disk_dir_name) - 1] = '\0';
                    break;
                default:
                    fprintf(stderr, "Error: Default case reached.\n");
                    exit(UNEXPECTED_ERROR);
                    break;
            }
            looking_for_param = FALSE;
        }
        else {
            fprintf(stderr, "Error: Incorrect usage.\n");
            exit(INCORRECT_USAGE);
        }
    }

    if (looking_for_param) {
        fprintf(stderr, "Error: Expected parameter for the -%c flag.\n",
            current_flag);
        exit(MISSING_PARAM);
    }

    //  Verify correct combination of flags
    //  1.  Must specify either -p or -u
    //  2.  Can only specify -o if -p used (O only if P).
    //  (i.e., (P XOR U) and (O -> P))
    if (!(p_flag ^ u_flag)) {
        fprintf(stderr, "Error: Must specify -p or -u (pack or unpack).\n");
        exit(INCORRECT_USAGE);
    }

    if (o_flag && !p_flag) {
        fprintf(stderr, "Error: Can only specify -o flag if -p flag is used.\n");
        exit(INCORRECT_USAGE);
    }
}

void pack() {
    printf("pack()\n");

    //  Set disk file name to default if not specified by user
    if (!o_flag) {
        disk_file_name = DEFAULT_DISK_FILE_NAME;
    }

    //  Check that neither string is null at this point
    if (disk_dir_name == NULL || disk_file_name == NULL) {
        fprintf(stderr, "Error: Disk directory name or file name is null.\n");
        exit(UNEXPECTED_ERROR);
    }

    //  Verify specified disk directory contents are correct
    DiskDirectory * disk_dir = getDiskDirectory(disk_dir_name);

    //  Create a ByteFrost Disk (TODO convert into global variable)
    FILE * disk = createBFSDiskv1(disk_file_name);

    printf("DiskDirectory struct:\n");
    printf("\tnum_files: %d\n", disk_dir->num_files);

    //  Populate boot block range
    writeBootFile(disk_dir, disk);

    //  Add files to the disk
    for (int i = 0; i < disk_dir->num_files; i++) {
        writeFileToDisk(disk_dir, i, disk);
    }

    printf("Finished packing directory '%s' into disk file '%s'.\n",
        disk_dir_name, disk_file_name);
}

void unpack() {
    printf("unpack()\n");
}

DiskDirectory * getDiskDirectory(char * directory_name) {
    printf("getDiskDirectory()\n");
    //  Attempt to open the disk directory
    DIR * disk_dir;
    struct dirent *disk_dp;

    if ((disk_dir = opendir(directory_name)) == NULL) {
        fprintf(stderr, "Error: pack(): Couldn't open directory %s.\n",
            directory_name);
        exit(DIRECTORY_FAIL);
    }

    //  Verify directory contents match requirements:
    //  disk_dir/
    //      files/
    //          #   max. 16 files; each filename can be at most 14 
    //          #   chars long and be at most 31.5 KB in size
    //      boot.bin
    //          #   (machine code to run when ByteFrost boots; this 
    //          #   file can be at most 2 KB in size)

    int files_dir_found = FALSE;
    int boot_file_found = FALSE;

    printf("Directory %s contents:\n", directory_name);
    while ((disk_dp = readdir(disk_dir)) != NULL) {
        printf("\t%s\n", disk_dp->d_name);
        
        if (strcmp(disk_dp->d_name, FILES_DIR_NAME) == 0) {
            printf("Files directory found!\n");
            files_dir_found = TRUE;
        }
        else if (strcmp(disk_dp->d_name, BOOT_FILE_NAME) == 0) {
            printf("Boot file found!\n");
            boot_file_found = TRUE;
        }
    }

    //  Close disk directory
    closedir(disk_dir);

    if (!files_dir_found) {
        fprintf(stderr, "Error: Missing '%s' directory in %s directory.\n",
            FILES_DIR_NAME, directory_name);
        exit(MISSING_FILES_DIR);
    }

    if (!boot_file_found) {
        fprintf(stderr, "Error: Missing '%s' file in %s directory.\n",
            BOOT_FILE_NAME, directory_name);
        exit(MISSING_BOOT_FILE);
    }

    //  Verify that files directory contains at most 16 files and that each
    //  file has a filename that is at most 14 chars long and a size of at most
    //  31.5 KB

    //  Allocate a DiskDirectory struct
    DiskDirectory * disk_directory = 
        (DiskDirectory *) malloc(sizeof(DiskDirectory));

    //  Initialize num_files
    disk_directory->num_files = 0;

    //  Get files path ("{directory_name}/files")
    char files_path[MAX_FILE_NAME_LEN];
    strcpy(files_path, directory_name);
    strcat(files_path, "/");
    strcat(files_path, FILES_DIR_NAME);

    printf("files path: %s\n", files_path);

    DIR * files_dir;
    struct dirent *files_dp;

    if ((files_dir = opendir(files_path)) == NULL) {
        fprintf(stderr, "Error: pack(): Couldn't open directory %s.\n",
            directory_name);
        exit(DIRECTORY_FAIL);
    }

    int num_files = 0;
    FILE * current_file = NULL;
    char current_file_path[MAX_FILE_NAME_LEN];
    int current_file_size;

    int max_file_size = MAX_FILE_SIZE;
    while ((files_dp = readdir(files_dir)) != NULL) {
        if (strcmp(files_dp->d_name, ".") == 0 
            || strcmp(files_dp->d_name, "..") == 0) 
        {
            //  Skip current directory and parent directory hardlinks
            continue;
        }
        printf("\t%s\n", files_dp->d_name);

        char ** file_name_split = splitFileExtension(files_dp->d_name);

        printf("file_name_split[0] = %s\tfile_name_split[1] = %s\n",
            file_name_split[0], file_name_split[1]);

        num_files++;

        //  Check number of files
        if (num_files > MAX_NUM_FILES) {
            fprintf(stderr, "Error: There cannot be more than %d files in the",
                MAX_NUM_FILES);
            fprintf(stderr, " '%s' directory.\n", FILES_DIR_NAME);
            exit(TOO_MANY_FILES);
        }

        //  Check current file name's length
        if (strlen(file_name_split[0]) > FILE_NAME_CHAR_LIMIT) {
            fprintf(stderr, "Error: File '%s' has a name that is longer than",
                file_name_split[0]);
            fprintf(stderr, " %d characters.\n", FILE_NAME_CHAR_LIMIT);
            exit(LONG_FILE_NAME);
        }

        //  Check current file's size

        //  Get current file's path
        strcpy(current_file_path, files_path);
        strcat(current_file_path, "/");
        strcat(current_file_path, files_dp->d_name);

        printf("Current file's path: %s\n", current_file_path);

        //  Open current file
        if ((current_file = fopen(current_file_path, "r")) == NULL) {
            fprintf(stderr, "Error: Couldn't open file '%s'.\n", 
                current_file_path);
            exit(FILE_FAIL);
        }

        //  Allocate string for filename in DiskDirectory
        disk_directory->file_names[disk_directory->num_files] =
            (char *) malloc((strlen(file_name_split[0]) + 1)/ sizeof(char));
        strcpy(disk_directory->file_names[disk_directory->num_files],
            file_name_split[0]);

        printf("disk_directory->file_names[%d] = %s\n", 
            disk_directory->num_files, 
            disk_directory->file_names[disk_directory->num_files]);

        //  Add current file to DiskDirectory
        disk_directory->files[disk_directory->num_files] = current_file;

        //  Set file type based on file extension
        disk_directory->types[disk_directory->num_files] = 
            getFileType(file_name_split[1]);

        printf("File type: %d\n", 
            disk_directory->types[disk_directory->num_files]);

        

        //  Go to end of file
        fseek(current_file, 0, SEEK_END);

        //  Get size by position
        current_file_size = ftell(current_file);

        //  Reset file position to beginning
        rewind(current_file);

        printf("%s size:\t%d bytes\n", current_file_path, current_file_size);

        if (current_file_size > max_file_size) {
            fprintf(stderr, "Error: File '%s' is larger than %d KB.\n",
                current_file_path, MAX_FILE_SIZE / 1024);
            exit(FILE_TOO_BIG);
        }

        //  Set DiskDirectory's file size for this file
        disk_directory->file_sizes[disk_directory->num_files] = current_file_size;
        //  Update num_files
        disk_directory->num_files++;
    }

    //  Verify that the boot file is at most 2 KB in size
    char boot_file_path[MAX_FILE_NAME_LEN];
    FILE * boot_file;
    int boot_file_size;
    int max_boot_file_size = MAX_BOOT_FILE_SIZE;

    strcpy(boot_file_path, directory_name);
    strcat(boot_file_path, "/");
    strcat(boot_file_path, BOOT_FILE_NAME);

    printf("Boot file path: %s\n", boot_file_path);

    if ((boot_file = fopen(boot_file_path, "r")) == NULL) {
        fprintf(stderr, "Error: Couldn't open file '%s'.\n", 
            boot_file_path);
        exit(FILE_FAIL);
    }

    //  Add boot file to DiskDirectory
    disk_directory->boot_file = boot_file;

    //  Go to end of file
    fseek(boot_file, 0, SEEK_END);

    //  Get size by position
    boot_file_size = ftell(boot_file);

    //  Reset file position to beginning of file
    rewind(boot_file);

    printf("%s size:\t%d\n", boot_file_path, boot_file_size);

    if (boot_file_size > max_boot_file_size) {
        fprintf(stderr, "Error: File '%s' is larger than %d KB.\n",
            boot_file_path, MAX_BOOT_FILE_SIZE / 1024);
        exit(FILE_TOO_BIG);
    }

    //  Close directories
    closedir(files_dir);

    return disk_directory;
}

FILE * createBFSDiskv1(char * disk_name) {
    printf("createBFSDiskv1()\n");
    //  Input validation
    if (disk_name == NULL) {
        fprintf(stderr, "Error: createBFSDiskv1(): disk_name is null.\n");
        exit(UNEXPECTED_ERROR);
    }

    //  Create new ByteFrost FileSystem v1 disk file in the current directory
    printf("Disk file name: %s\n", disk_name);
    
    //  1.  Create a 512KB file and fill it initially with 0s.
    int disk_size = DISK_SIZE_KB;     //  Size of disk in KB

    FILE * disk;

    if ((disk = fopen(disk_name, "w+b")) == NULL) {
        fprintf(stderr, "Error: Couldn't create disk file.\n");
        exit(UNEXPECTED_ERROR);
    }

    //  Allocate a 1KB buffer of all 0s
    char buf[1024] = {0};

    int bytesWritten;
    for (int i = 0; i < disk_size; i++) {
        if ((bytesWritten = fwrite(buf, sizeof(char), sizeof(buf)/sizeof(buf[0]), disk)) != 1024) {
            fprintf(stderr, "Error: Failed to write 0s to disk.");
            fprintf(stderr, " bytesWritten = %d. Error: %s\n", bytesWritten,
                strerror(errno));
            exit(UNEXPECTED_ERROR);
        }

        // printf("Bytes written: %d\n", bytesWritten);
    }

    printf("Disk position at end of disk: %ld\n", ftell(disk));

    return disk;
}

void writeBootFile(DiskDirectory * disk_dir, FILE * disk) {
    printf("writeBootFile()\n");
    //  Input validation
    if (disk_dir == NULL || disk == NULL || disk_dir->boot_file == NULL) {
        fprintf(stderr, "Error: writeBootFile(): a parameter was null.\n");
        exit(UNEXPECTED_ERROR);
    }

    //  Read block chunks from the boot file into the disk's boot block range
    char buf[BLOCK_SIZE];       //  Block buffer
    int bytesRead;              //  Number of bytes read in each iteration

    //printf("Size of char: %ld\n", sizeof(char));

    //  Rewind disk and boot file position to beginning
    rewind(disk);
    rewind(disk_dir->boot_file);

    while ((bytesRead = fread(buf, sizeof(char), BLOCK_SIZE/sizeof(char), disk_dir->boot_file)) > 0) {
        //  Write boot file contents to the disk
        printf("bytesRead: %d\n", bytesRead);
        fwrite(buf, sizeof(char), bytesRead/sizeof(char), disk);
    }
}

void writeFileToDisk(DiskDirectory * disk_dir, int fileIndex, FILE * disk) {
    printf("writeFileToDisk()\n");
    //  Input validation
    if (disk_dir == NULL || disk == NULL) {
        fprintf(stderr, "Error: a parameter was null.\n");
        exit(UNEXPECTED_ERROR);
    }

    //  Creating a new file - allocate an inode block
    int inode_block = allocateInode(disk);

    if (inode_block == -1) {
        fprintf(stderr, "Error: Ran out of inode blocks.\n");
        exit(UNEXPECTED_ERROR);
    }

    printf("Allocated inode block %d (starting at %d) for file %s.\n", 
        inode_block, FIRST_INODE_BLOCK, disk_dir->file_names[fileIndex]);

    //  Set a directory entry to this file's name and inode block pointer
    allocateDirectoryEntry(disk_dir->file_names[fileIndex], inode_block, disk);

    //  Update the inode's block with the file's info (excluding size metadata)
    //  (i.e., set inode's file type field)
    setInodeMetadata(disk_dir->types[fileIndex], inode_block, disk);

    //  Write data to file 
    //      (allocate data block -> 
    //      update inode data block pointers and size -> write to data block
    //      -> repeat until done)

    //  Rewind file's position
    FILE * file = disk_dir->files[fileIndex];
    int file_size = disk_dir->file_sizes[fileIndex];

    rewind(file);

    //  Read from file in block-sized chunks
    char buf[BLOCK_SIZE];
    int bytesRead;
    int data_block;
    int totalBytes = 0;                 //  Total number of bytes written
    int numBlocks = 0;                  //  Total number of data blocks allocated for file
    char inode_data_block_field[1];     //  Buffer to write data block to inode
    char inode_size_bytes_field[2];     //  Buffer to write size in bytes to inode
    char inode_size_blocks_field[1];    //  Buffer to write size in blocks to inode

    while ((bytesRead = fread(buf, 1, BLOCK_SIZE, file)) > 0) {
        //  TODO: May need to convert data for text files (e.g., new line may
        //  have a different value in the ByteFrost than in ASCII, so may need
        //  to check for this to correctly represent text files in the BFS)

        //  Allocate a data block
        data_block = allocateDataBlock(disk);
        inode_data_block_field[0] = (char) data_block;

        //  Update last data block pointer in inode
        fseek(disk, 
            (inode_block * BLOCK_SIZE) + FIRST_DATA_PTR_OFFSET + numBlocks, 
            SEEK_SET);
        
        if (fwrite(inode_data_block_field, 1, 1, disk) != 1) {
            fprintf(stderr, "Error: Failed to update data block pointer in inode.\n");
            exit(UNEXPECTED_ERROR);
        }
        
        numBlocks++;

        //  Write data in buffer to data block
        fseek(disk, data_block * BLOCK_SIZE, SEEK_SET);
        
        if (fwrite(buf, 1, bytesRead, disk) != bytesRead) {
            fprintf(stderr, "Error: Failed to write file data to data block.\n");
            exit(UNEXPECTED_ERROR);
        }

        totalBytes += bytesRead;

        //  Update inode data block pointers + size
        fseek(disk, inode_block * BLOCK_SIZE, SEEK_SET);

        //  Store totalBytes as HIGH byte, LOW byte
        //  ByteFrost then can be Big-Endian in most cases, and little-endian
        //  only when storing instructions in binary files (where you store
        //  the low byte first, then the high byte for each 2-byte instruction)
        inode_size_bytes_field[0] = (char) (logicalShiftRight(totalBytes & 0xFF00, 8));
        inode_size_bytes_field[1] = (char) (totalBytes & 0x00FF);
        
        inode_size_blocks_field[0] = (char) numBlocks;
        
        if (fwrite(inode_size_bytes_field, 1, 2, disk) != 2) {
            fprintf(stderr, "Error: Failed to update inode's file size in bytes field.\n");
            exit(UNEXPECTED_ERROR);
        }
        
        if (fwrite(inode_size_blocks_field, 1, 1, disk) != 1) {
            fprintf(stderr, "Error: Failed to update inode's file size in blocks field.\n");
            exit(UNEXPECTED_ERROR);
        }
    }

    if (totalBytes != file_size) {
        fprintf(stderr, "Error: writeFileToDisk(): wrote %d bytes instead of %d bytes.\n",
            totalBytes, file_size);
        exit(UNEXPECTED_ERROR);
    }
}

/**
 * @brief Allocate an inode block
 * 
 * @param disk 
 * @return int Returns an inode block pointer
 */
int allocateInode(FILE * disk) {
    printf("allocateInode()\n");
    //  Input validation
    if (disk == NULL) {
        fprintf(stderr, "Error: a parameter was null.\n");
        exit(UNEXPECTED_ERROR);
    }

    //  Update disk file position to inode freemap block
    fseek(disk, INODE_FREE_MAP_BLOCK * BLOCK_SIZE, SEEK_SET);

    //printf("position at start: %ld\n", ftell(disk));

    char cur_free_inodes[1];   //  Current byte of the freemap
    int free_inode = 0;
    int bytesRead;

    for (int i = 0; i < INODE_FREE_MAP_LEN; i++) {
        if ((bytesRead = fread(cur_free_inodes, sizeof(char), 1/sizeof(char), disk)) != 1) {
            fprintf(stderr, "Error: fread() failed; bytesRead = %d. error: %s\n", 
                bytesRead, strerror(errno));
            exit(UNEXPECTED_ERROR);
        }

        //printf("position after fread(): %ld\n", ftell(disk));

        printf("inode freemap[%d] = 0x%x\n", i, cur_free_inodes[i]);

        //  Check if there are any free inode bits in the current byte
        if (cur_free_inodes[0] == 0xFF)
            continue;

        //  Find first free inode   (start at MSB then go to LSB)
        unsigned char mask = 0x80;   //  1000_0000
        while (mask != 0) {
            if ((mask & cur_free_inodes[0]) == 0) {
                //  Found free inode!
                //  Update inode freemap

                //printf("position before updating inode: %ld\n", ftell(disk));
                
                //  Move file position back to this freemap byte
                fseek(disk, -1, SEEK_CUR);

                //printf("position after seek: %ld\n", ftell(disk));

                //  Set byte such that inode is used up
                cur_free_inodes[0] = mask | cur_free_inodes[0];

                //printf("Modified inode free map byte: 0x%x\n", (unsigned int)cur_free_inodes[0]);

                fwrite(cur_free_inodes, 1, 1, disk);

                //printf("position after updating inode: %ld\n", ftell(disk));

                // fseek(disk, -1, SEEK_CUR);

                // fread(cur_free_inodes, 1, 1, disk);

                // printf("Modified inode free map byte in disk: 0x%x\n", (unsigned int)cur_free_inodes[0]);

                return free_inode + FIRST_INODE_BLOCK;
            }
            free_inode++;
            mask = logicalShiftRight(mask, 1);
        }
    }

    //  All inodes are used up
    return -1;
}

int allocateDataBlock(FILE * disk) {
    printf("allocateDataBlock()\n");

    //  Input validation
    if (disk == NULL) {
        fprintf(stderr, "Error: a parameter was null.\n");
        exit(UNEXPECTED_ERROR);
    }

    //  Update disk's position to data free map block
    fseek(disk, DATA_FREE_MAP_BLOCK * BLOCK_SIZE, SEEK_SET);

    char cur_free_map[1];   //  Current byte of the freemap
    int free_data_block = 0;
    int bytesRead;

    for (int i = 0; i < DATA_FREE_MAP_LEN; i++) {
        if ((bytesRead = fread(cur_free_map, 1, 1, disk)) != 1) {
            fprintf(stderr, "Error: fread() failed; bytesRead = %d. error: %s\n",
                bytesRead, strerror(errno));
            exit(UNEXPECTED_ERROR);
        }

        printf("data freemap[%d] = 0x%x\n", i, cur_free_map[0]);

        //  Check if there are any free data map bits in the current byte
        if (cur_free_map[0] == 0xFF)
            continue;

        //  Find first free data block (start at MSB then go to LSB)
        unsigned char mask = 0x80;  //  1000_0000
        while (mask != 0) {
            if ((mask & cur_free_map[0]) == 0) {
                //  Found free data block!
                //  Update data block freemap

                //  Move file position back to this freemap byte
                fseek(disk, -1, SEEK_CUR);

                //  Set byte such that block is used up
                cur_free_map[0] = mask | cur_free_map[0];

                fwrite(cur_free_map, 1, 1, disk);

                return free_data_block + FIRST_DATA_BLOCK;
            }
            free_data_block++;
            mask = logicalShiftRight(mask, 1);
        }
    }

    //  All data blocks are used up
    return -1;
}

int logicalShiftRight(int x, int shiftAmt) {
    //  Based on https://stackoverflow.com/a/16981120
    int size = sizeof(int) * 8;

    int result = (x >> shiftAmt) 
        & ~((1 << (size - 1)) >> (shiftAmt - 1));

    printf("x: 0x%x\tx >> %d: 0x%x\n", x, shiftAmt, result);

    return result;
}

/**
 * @brief Splits the given file name and returns an array of length 2 for the
 * filename before the extension and the extension
 * 
 * @param filename 
 * @return char ** 
 */
char ** splitFileExtension(char * filename) {
    //  Input validation
    if (filename == NULL) {
        fprintf(stderr, "Error: splitFileExtension(): Parameter was null.\n");
        exit(UNEXPECTED_ERROR);
    }

    char * filename_copy = 
        (char *) malloc((strlen(filename) + 1) / sizeof(char));

    strcpy(filename_copy, filename);

    printf("filename_copy: %s\n", filename_copy);

    int name_length = strlen(filename_copy);

    char ** stringArray = (char **) malloc(2 * sizeof(char *));

    int dotPosition = 0;
    for (int i = 0; i < name_length; i++) {
        if (filename_copy[i] != '.')
            dotPosition++;
        else
            break;
    }

    printf("dot position: %d\n", dotPosition);
    
    //  Replace '.' character with end sentinel
    // if (dotPosition < strlen(filename_copy))
    filename_copy[dotPosition] = '\0';

    stringArray[0] = 
        (char *) malloc((strlen(filename_copy) + 1) / sizeof(char));

    strcpy(stringArray[0], filename_copy);

    if (dotPosition < name_length) {
        stringArray[1] =
            (char *) malloc((strlen(&filename_copy[dotPosition + 1]) + 1) / sizeof(char));

        strcpy(stringArray[1], &filename_copy[dotPosition + 1]);
    }
    else {
        printf("No extension\n");
        //  No extension - put in empty string
        stringArray[1] = (char *)malloc(sizeof(char));
        stringArray[1][0] = '\0';
    }

    free(filename_copy);

    return stringArray;
}

/**
 * @brief Returns the file type of the file based on its extension
 * 
 * @param extension String of the file's extension (not including dot)
 * @return FileType the type of the file
 */
FileType getFileType(char * extension) {
    //  Input validation
    if (extension == NULL) {
        fprintf(stderr, "Error: getFileType(): Parameter was null.\n");
        exit(UNEXPECTED_ERROR);
    }

    if (strcmp(extension, "txt") == 0) {
        return TEXT;
    }
    if (strcmp(extension, "asm") == 0) {
        return ASM;
    }
    if (strcmp(extension, "mlg") == 0) {
        return MLG;
    }
    if (strcmp(extension, "bin") == 0) {
        return BIN;
    }

    return UNKNOWN;
}

void allocateDirectoryEntry(char * filename, int inode_block, FILE * disk) {
    printf("allocateDirectoryEntry()\n");

    //  Input validation
    if (filename == NULL || disk == NULL) {
        fprintf(stderr, 
            "Error: allocateDirectoryEntry(): filename or disk was null.\n");
        exit(UNEXPECTED_ERROR);
    }

    //  Check that inode block is within inode block range
    if (inode_block < FIRST_INODE_BLOCK || inode_block > LAST_INODE_BLOCK) {
        fprintf(stderr, "Error: allocateDirectoryEntry(): block %d is not",
            inode_block);
        fprintf(stderr, " a valid inode block.\n");
        exit(UNEXPECTED_ERROR);
    }

    //  Allocate a directory entry

    //  Set disk file position to the directory block
    fseek(disk, DIRECTORY_BLOCK * BLOCK_SIZE, SEEK_SET);

    printf("Current disk file positon: %ld\n", ftell(disk));

    //  Read the directory block
    char directory_block[BLOCK_SIZE];
    int bytesRead;

    if ((bytesRead = fread(directory_block, 1, BLOCK_SIZE, disk)) != BLOCK_SIZE) {
        fprintf(stderr, "Error: Failed to read directory block. (bytesRead = %d)",
            bytesRead);
        fprintf(stderr, " error: %s\n", strerror(errno));
        exit(UNEXPECTED_ERROR);
    }

    //  Move file pointer position back to start of directory bock
    fseek(disk, -BLOCK_SIZE, SEEK_CUR);

    printf("Current disk file positon: %ld\n", ftell(disk));

    //  Iterate through directory entries in directory block
    char current_entry[DIRECTORY_ENTRY_SIZE];
    int bytesWritten;
    for (int i = 0; i < MAX_NUM_FILES; i++) {
        //  Get current directory entry
        for (int j = 0; j < DIRECTORY_ENTRY_SIZE; j++) {
            current_entry[j] = directory_block[(i * DIRECTORY_ENTRY_SIZE) + j];
        }

        //  Check that entry is empty
        if (current_entry[14] == 0 && current_entry[15] == 0) {
            //  Found an empty directory entry!
            printf("Directory entry %d is available. Allocating.\n", i);

            //  Set directory entry's filename

            //  If filename length is filename char limit, then don't include
            //  the 0-sentinel, but if it's shorter, do include it
            int filename_length = 
                (strlen(filename) >= FILE_NAME_CHAR_LIMIT) 
                ? FILE_NAME_CHAR_LIMIT
                : strlen(filename) + 1;
            for (int j = 0; j < filename_length; j++) {
                current_entry[j] = filename[j];
            }

            //  Set directory entry's inode block pointer (only need to use 1
            //  byte since inode blocks are all less than 256)
            current_entry[DIRECTORY_ENTRY_SIZE - 1] = inode_block;

            //  Write current entry to disk

            fseek(disk, i * DIRECTORY_ENTRY_SIZE, SEEK_CUR);

            printf("Current disk file position: %ld\n", ftell(disk));

            if ((bytesWritten = fwrite(current_entry, 1, DIRECTORY_ENTRY_SIZE, disk)) != DIRECTORY_ENTRY_SIZE) {
                fprintf(stderr, "Error: Failed to write directory entry.");
                fprintf(stderr, " bytesWritten = %d. Error: %s\n", 
                    bytesWritten, strerror(errno));
                exit(UNEXPECTED_ERROR);
            }
            return;
        }
    }

    fprintf(stderr, "Error: No directory entry is available.\n");
    exit(UNEXPECTED_ERROR);
}

void setInodeMetadata(FileType type, int inode_block, FILE * disk) {
    printf("setInodeMetadata()\n");

    //  Input validation
    if (disk == NULL) {
        fprintf(stderr, "Error: setInodeMetadata(): disk file is null.\n");
        exit(UNEXPECTED_ERROR);
    }
    if (inode_block < FIRST_INODE_BLOCK || inode_block > LAST_INODE_BLOCK) {
        fprintf(stderr, "Error: allocateDirectoryEntry(): block %d is not",
            inode_block);
        fprintf(stderr, " a valid inode block.\n");
        exit(UNEXPECTED_ERROR);
    }

    //  Set metadata in specified inode block

    //  Move disk position to inode block and to its file type byte
    fseek(disk, (inode_block * BLOCK_SIZE) + FILE_TYPE_OFFSET, SEEK_SET);

    printf("Current disk file position: %ld\n", ftell(disk));

    //  Write type
    char buf[1] = {type};
    int bytesWritten;
    if ((bytesWritten = fwrite(buf, 1, 1, disk)) != 1) {
        fprintf(stderr, "Error: Failed to write file type to inode.");
        fprintf(stderr, " bytesWritten = %d. Error: %s\n", 
            bytesWritten, strerror(errno));
        exit(UNEXPECTED_ERROR);
    }
    printf("Set inode block %d's type to %d.\n", inode_block, type);
}