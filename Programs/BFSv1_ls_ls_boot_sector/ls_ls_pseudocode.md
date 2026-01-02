#   `ls -ls` Pseudocode

```c
struct DirectoryEntry {
    char filename[14];
    uint16_t inode_block_sector;
};

struct Inode {
    uint16_t file_size_bytes;
    uint8_t file_size_blocks;
    uint8_t file_type;
    uint16_t data_block_pointers[126];
};

void load_sector(uint16_t sector, uint8_t page) {
    //  Load sector into the specified page in RAM.
}

int main() {
    printf("$ ls -ls\n");

    //  Memory use:
    //  Page 0x30: Directory block
    //  Page 0x31 - 0x46: Inode array
    //      *   Directory entry index in directory block is used as index to
    //          this array.
    //  Stack pointer at 0xDFFF.
    //  Stack breakdown:
    //  SP, SP - 1: total_blocks (little-endian)
    
    //  1.  Load block 8 that contains the disk root (and only) directory block
    //      into RAM at page 0x30.
    load_sector(8, 0x30);

    //  2.  Set total_blocks (a 16-bit counter as there are 2048 total blocks in
    //      the disk) to 0.
    uint16_t total_blocks = 0;

    //  3.  For each directory entry (0 through 15):
    //      1.  If the inode block pointer of the current entry is 0, skip to
    //          the next entry.
    //      2.  Load the inode block of the current entry at page 0x31 + i
    //          (where i is the current directory block entry index).
    //      3.  Increment total_blocks by the file size field (in blocks) in
    //          that inode.
    DirectoryEntry * entry = 0x3000;
    for (int i = 0; i < 16; i++, entry++) {
        if (entry->inode_block_sector == 0) continue;
        uint8_t inode_page = 0x31 + i;
        load_sector(entry->inode_block_sector, inode_page);
        
        Inode * entry_inode = inode_page << 8;
        total_blocks += entry_inode->file_size_blocks;
    }

    //  4.  Print total blocks used in the disk (since the disk only has one
    //      directory).
    printf("total %hu\n", total_blocks);

    //  5.  For each directory entry (0 through 15):
    //      1.  If the inode block pointer of the current entry is 0, skip to
    //          the next entry.
    //      2.  Read the file size field in blocks of this entry's inode and
    //          print it (2 chars).
    //      3.  Print space.
    //      4.  Read the file size field in bytes of this entry's inode and
    //          print it (4 chars, requires two GPRs).
    //      5.  Print space.
    //      6.  Print the filename.
    //      7.  Print newline.
    entry = 0x3000;
    Inode * entry_node = (0x31 + i) << 8;
    for (int i = 0; i < 16; i++, entry++, entry_node++) {
        if (entry->inode_block_sector == 0) continue;

        uint8_t file_size_blocks = entry_node->file_size_blocks;
        printf("%hu ", file_size_blocks);

        uint16_t file_size_bytes = entry_node->file_size_bytes;
        printf("%hu ", file_size_bytes);

        printf("%s\n", entry->filename);
    }

    //  6.  Print "$ "
    printf("$ ");
}
```