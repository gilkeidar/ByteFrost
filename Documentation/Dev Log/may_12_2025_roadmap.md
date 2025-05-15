#   ByteFrost Development Log - Current Roadmap

May 12, 2025

##  Discussion

First, we shall discuss the present roadmap for the ByteFrost (that is, the set
of goals to aspire to achieve in the next few months). This is essentially a
roadmap towards completing the majority (if not all) goals of the ByteFrost v2
framework.

The overarching goals are as follows:
1.  Modify / augment the ByteFrost (v2) ISA to support the new 16-bit
    architecture.
    *   This mainly (if not entirely) focuses on the expansion and streamlining
        of the ByteFrost (v2) ISA's addressing modes discussed in the two
        previous logs.
    *   **Note:** A complete discussion of addressing modes will be contained in 
        this development log.
    *   **Note:** Implicitly included here are some convention decisions
        regarding memory, specifically regarding the calling convention of the
        ByteFrost. This includes the type of stack (preferably full-descending),
        endianess of multi-byte data (preferably little-endian, as instructions
        are already stored in little-endian format), and the details of the
        calling convention (what a stack frame looks like, what the caller
        pushes on the stack, what the callee pushes on the stack, etc.)
2.  Implementing a simple (Apple II-style) shell.
    *   This is actually a fairly involved goal, not so much because of the
        complexity of the shell software, but rather because of the many
        foundational pieces that must first be implemented / refined in order
        for such software to be written.
    *   For this goal, it is pertinent to solidify the hardware / software stack
        in terms of:
        *   **Assembler** - making sure that the assembler is robust, supports 
            the entire v2 ISA, and can shift label addresses by a constant since
            programs may be loaded at different addresses (though necessarily,
            each program may only be loaded at an address particular to that
            program)
        *   **Disk** - making sure that reading / writing sectors from / to disk
            is robust (Arduino disk controller <-> ByteFrost interface)
        *   **Assembly to Execution Pipeline** - creating a software pipeline
            / toolchain to simplify and streamline writing an assembly program
            to running it on the ByteFrost (whether the program is running in
            ROM, RAM, expected to be called by the shell, etc.) 

We shall discuss these two overarching goals in detail in the following 
sections.

### Modifying / Augmenting the ByteFrost (v2) ISA (On Addressing Modes)

####    Regarding the Stack and Calling Convention

### Implementing a Simple (Apple II-style) Shell

First, let us give a high-level specification for what the shell should be and
what it should be capable of doing:

**High-Level Specification:** The shell should be a piece of software that
implements a simple loop of waiting for the user to input a command (via a
keyboard) and, upon receipt of this command, performing it. 

The shell should recognize commands to do at least the following: (1) load a
program into RAM, and (2) running a program in RAM (and when that program
completes, control should return to the shell; i.e., it should progress to 
another iteration of the input command loop).

Nice to haves: additional (3) disk / filesystem commands and (4) misc. commands,
such as:
1.  Disk / Filesystem (BFSv1)
    1.  An `ls` program (print a listing of files in the root directory)
    2.  A `cat` program (print the contents of a specified file)
        *   **Note:** Requires a convention for passing command-line arguments
            to a program, unless the program (while running) asks for arguments.
            (this could be done by the shell, i.e. it could parse the string of
            characters inputted by the user (though the interface for how it
            would get this string is still undefined) as 
            `[command] {arg1} {arg2} {...}`. It would then create the command
            line argument array (`argv`) on the top of the stack, then "call"
            the relevant program after it loaded it into RAM. In this way, when
            the called program runs, the SP points at the low byte of the return
            address (in the shell), and at `SP + 2` could be stored `argc` and
            after that (probably `SP + 3` since `argc` likely is much less than
            `255`) the `char ** argv` variable; the actual `argv` array can be
            immediately above it (`SP + 5` since addresses are 2 bytes); I
            believe this is how command-line arguments are stored in Linux; in
            effect, the shell calls the `main()` function of the called program.
            The only difference is that no new process is created, the shell
            just jumps to the called program and hopes that it will eventually
            return from `main()`, at which point the shell will resume (Direct
            Execution model))
2.  Misc.
    1.  Execution state summary (print current register values)

Let us just consider the first two commands for now and the main shell input
loop. The code for the shell could look something like this: (very simplified)

```c
//  Max command length (note the exciting possibility of buffer overflow!
//  Of course, the effectiveness of such an attack depends on where the input
//  buffer would be stored; it is also not guaranteed that the runShell()
//  function would have a conventional stack frame)
//  Note: This contains the + 1 for null terminator (?) may not be necessary
//  given that length is known, but might be good general practice
#define MAX_COMMAND_LENGTH  256

void runShell() {
    //  Command input buffer. Question: Where is this stored?
    char input[MAX_COMMAND_LENGTH];

    while (TRUE) {
        //  Get input from "stdin" (keyboard) - however, no file descriptor
        //  abstractions exist
        // getline(input, MAX_COMMAND_LENGTH, stdin);
        //  There's a question of how to tell the keyboard to write the seen
        //  characters to the input buffer. It likely makes more sense that
        //  there is one buffer in MMIO somewhere that the keyboard's controller
        //  will write directly into, or perhaps there will be a "getChar()"
        //  interface (like readSector() for the disk controller) via MMIO to
        //  read a character. I'm not sure yet; point is, the shell will need
        //  to read a line from user input until the user presses the Enter key.
        getLineFromKeyboard(input, MAX_COMMAND_LENGTH);

        //  Replace spaces in the input buffer with \0 to create a split array.

        if (strcmp(input, "LOAD") == 0) {
            //  Load command; read the rest of the input to identify the 
            //  requested program.

            //  Copy the requested program from disk into RAM starting at the
            //  program's starting address.
            //  Question: What starting address should be used?
            //      Option 1: A known starting address that is the same for all
            //          shell programs.
            //          *   This means that a program must be compiled by the
            //              assembler ahead of time as a "shell program" so that
            //              all of its addresses (in labels) are offset so that
            //              assembler address 0 aligns with the known starting
            //              address.
            //      Option 2: The starting address is contained in the header of
            //          the shell program.
            //          *   If only shell programs have headers though, in
            //              effect this is the same as needing to compile the
            //              program as a shell program ahead of time since
            //              otherwise, naively loading the program and jumping
            //              to its first instruction would jump to the first
            //              byte of the header which isn't an instruction.
            //              However, we can create a standard for all executable
            //              files to have the same header format (minimally, 2
            //              bytes specifying the address it should be loaded
            //              into). (Only exception to this is the ROM code that
            //              doesn't have to be loaded and the boot sector that
            //              is always loaded to address 0x2000 by the ROM code).

        }
        else if (strcmp(input, "RUN") == 0) {
            //  Jump to the loaded program address.
        }
    }
}
```

Let us just consider the first two commands for now. Implementing a shell with
these requires:
1.  Loading the shell program into memory (as well as programs the user wants
    to run using the shell).
    *   Essentially, there are at least two (or three) program loads that need
        to occur for the shell to load a specified program:
        0.  **ByteFrost is turned on / reset.** At this point, the PC points at
            `0x0000` and the first instruction of the ROM is fetched (after the
            technically implied first "NOP" instruction that the ByteFrost
            exectues after reset). The ROM boot code is then executed.
        1.  **ROM boot code loads boot sector to RAM.** The ROM boot code loads
            the first 8 sectors (2 KB) of the disk into RAM starting at address
            `0x2000` (first RAM address). It then jumps to this address.
        2.  **Boot sector runs.** At this point, there are two options:
            1.  The boot sector contains the shell, ergo the code that now runs
                is the shell code.
            2.  The boot sector could instead read a config file on disk (with
                known filename) that contains the name of a file that should be
                booted. The boot sector code searches for this file; if it
                exists, it loads this file after it (so, if the boot sector is
                `2 KB` in size, it covers addresses `0x2000 - 0x27ff`; hence,
                the file would be loaded at address `0x2800`). This file would
                then contain the shell code (it could start by copying itself
                down to address `0x2000`).
        3. **Optional: Boot sector loads shell executable file code into RAM.**
            *   This follows from option 2 in step 2.
        4.  **Shell runs.** User is provided with a shell prompt (`$ `) and
            provides a command, say `LOAD [programName]` (e.g., `LOAD zork`)
        5.  **Shell loads specified program into RAM.**
            *   The shell must find the program (if not found, print error),
                load into RAM (perhaps print error if too big? shell knows
                program size; provided in inode), 

2.  A keyboard peripheral (for the user to enter commands to the shell).

####    Assembly to Execution Pipeline

An important aspect that should be discussed now is the "Assembly to Execution"
pipeline / toolchain, i.e., what tools, files, and steps need to be taken in
order to write a ByteFrost Assembly file, compile it, put it on a disk, and
run it on the ByteFrost.

The steps involved are as follows:

1.  **Create an Assembly File.**
    *   **Step Detail:** Write a ByteFrost Assembly program using a text editor.
        The program should be written using ByteFrost Assembly (v2) (i.e., with
        the v2 Assembler).
    *   **Note:** A program on the ByteFrost can be run in multiple contexts; it
        can be either:
        1.  A ROM program.
            *   This means that the program is intended to be stored in an
                instruction ROM. Since the ROM covers addresses `0x0000` to
                `0x1FFF`, the address of the first instruction of the program
                can be `0x0000` (in other words, the assembler doesn't have to
                shift instruction addresses).
        2.  A RAM program.
            *   This means that the program is intended to be loaded into RAM
                (likely from disk). To load correctly, whatever program acts as
                the loader for this code (e.g., the ROM code, the boot sector
                code, the shell, etc.) needs to know the address at which to
                load the program (otherwise any branches will be incorrect as
                all branches are absolute branches, i.e. they specify the
                address to jump to).
            *   Use a preprocessor directive to specify the intended starting
                address.
    *   **File type(s):** `.asm` file (ByteFrost Assembly file)
        *   **Note:** Missed opportunity to use a `.bfa` (ByteFrost Assembly
            / Battle For Azeroth) file extension.
    *   **Tool(s):** Notepad++ (Recommended for BFA syntax highlighting), 
        VS Code
2.  **Create `.bin` File.**
    *   **Step Detail:** Compile the `.asm` file using the `Assembler v2`
        assembler, found in `\Assembler v2\ByteFrostAssembler\x64\Release`
        (the executable name is `ByteFrostAssembler.exe`) to produce a `.bin`
        (binary / machine language) file.
    *   **File type(s):** 
        *   **Input:**  `.asm` file.
        *   **Output:** `.bin` file.
    *   **Tool(s):** The ByteFrost Assembler (v2).
3.  **Create a BFSv1 `disk` File.**
    *   **Step Detail:** Create a BFSv1 (ByteFrost File System v1) `disk` file
        which can then be stored in an SD card using the FAT16/32 filesystem.
    0.  To do this, there are two relevant directories: the directory containing
        the intended contents of the `disk` file (call this the `src` directory)
        and the SD card root directory (call this the `card` directory).
    1.  The `src` directory must have the following structure:
        1.  `src/`
            1.  `files/`
                *   max. 16 files; each filename can be at most 14 chars long 
                    and be at most 31.5 KB in size
            2.  `boot.bin`
                *   (machine code to run when ByteFrost boots; this file can be
                    at most 2 KB in size)
        *   **Note:** If the compiled assembly file (`.bin`) should be stored in
            the boot sector, then it MUST be renamed to `boot.bin` and placed as
            the `boot.bin` file in the `src` directory. Otherwise, it should be
            stored under the `src/files/` directory.
    2.  Use the BFSv1 utility (located in `\Disk\FileSystem\BFSv1`, executable
        is called `bfs`) to generate a BFSv1 `disk` file using the `src` 
        directory contents. Use the pack command:
        *   `$  bfs -p [disk_dir]`
            *   The `-p` flag specifies the pack command (generate a `disk`
                file given a disk directory (`src`))
            *   Replace `[disk_dir]` with the name of the `src` directory
    3.  Place the generated `disk` file in the SD card `card` directory.
        *   Originally, the `card `directory could in principle contain multiple
            disk file with different names, and a `bf.con` config file would
            contain the name of the disk file that the Arduino disk controller
            should open, but this isn't currently implemented; hence, the SD
            card's root directory should contain a file called `disk` that 
            the disk controller will attempt to open (all other files on the SD
            card will be ignored.)