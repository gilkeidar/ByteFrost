#   ByteFrost FileSystem (BFS) Utility Specification

This command line utility is used to pack / unpack ByteFrost disk
files for the ByteFrost FileSystem version 1.

##  Interface

The `bfs` utility supports two use-cases:

`$  bfs -p [disk_dir] -o {disk_file_name}` is used to pack the
specified directory into a ByteFrost disk file with the name
specified using the `-o` flag. If the `-o` flag is not specified,
a default name will be used (`disk`).

The directory specified (`[disk_dir]`) **MUST** be structured as follows:

```bash
disk_dir/
    files/
        #   max. 16 files; each filename can be at most 14 
        #   chars long and be at most 31.5 KB in size
    boot.bin
        #   (machine code to run when ByteFrost boots; this 
        #   file can be at most 2 KB in size)
```

`$  bfs -u [disk_file_name]` is used to unpack the specified
ByteFrost fisk file into a directory formatted as above with
the name `[disk_file_name]_unpacked`

##  SD Card Format

The SD Card uses a FAT16/32 filesystem with the following required format:

```bash
SD_card/
    {ByteFrost disk file(s)}    
        #   can have more than one
    bf.con 
        #   file that contains only the name of used bytefrost 
        #   disk file
```