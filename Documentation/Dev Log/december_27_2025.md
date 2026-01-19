#   ByteFrost Development Log - On the Road to Version 2

December 27, 2025 - January 18, 2026

##  Overview

As of yesterday, December 26, 2025, all of the new and updated ISA instructions
described in the "Addressing Modes Proposal (v2)" development log (June 7 -
July 12, 2025) have been implemented. The proposal has been accurate almost in
its entirety, except on two points:
1.  The proposal treated the *Stack Pointer Count* control signal's semantics to
    mean that, if it is set, the `SP` will increment or decrement (depending on
    the *Stack Pointer Increment / Decrement* control signal's value) on the
    next rising clock edge. However, this was implemented differently, with the
    `SP` value being updated on the negative clock edge of the current cycle 
    (i.e., halfway through the cycle). This was resolved by changing the
    hardware to meet the expected semantics (i.e., the `SP` updates on the next
    rising clock edge).
    *   **Note:** This is currently only referring to `SP[L]` since the `SP[H]`
        register is not yet implemented using two 4-bit counter ICs.
2.  The proposal's implementation of the **AR Data Bus Load Enable LUT** assumed
    that the `PC Load Branch` (active low) signal would only be active during
    branch instructions, but this is not the case, and caused a bug detected
    when the `JSR`'s microcode was updated to the suggested 8-cycle(!)
    implementation, making it the longest instruction in the ByteFrost ISA.
    *   In the sixth cycle of the `JSR` instruction, the return address has 
        already been pushed onto the stack (in little-endian order) and now the
        `PC` was being set to the target address that is assumed to be stored in
        the `DP`; specifically, the `DHPC` is supposed to be set to `DP[H]`'s
        value. To do this, the following control signals are used:
        1.  *loadAR* = `1`
        2.  *loadARHorL* = `H` (`1`)
        3.  *PC Load* = `1`
    *   The problem here is that the hardware implementation of `PC Load Branch`
        is `~(PC Load (control signal) AND (branch should be taken))`, which has
        now become effectively `~(branch should be taken)`. To determine whether
        a branch instruction should be taken, no opcode check is performed first
        to ensure that the current instruction is, in fact, a branch
        instruction; instead, the condition bits of the current instruction
        string (bits `7:5`) are compared with the current ALU flags or treated
        as always taken or never taken, depending on the condition bit values.
        For `JSR` and `RTS`, being instructions that take no operands, all of
        the instruction bits besides the opcode have been (essentially
        arbitrarily) set to `0`, meaning that the condition bits were also set
        to `000`, meaning "branch always taken" (used by the unconditional `JMP`
        instruction).
    *   Hence, during this 6th cycle of `JSR`, the branch condition hardware
        erroneously set `PC Load Branch` to low (active), and the implementation
        of the **AR Data Bus Load Enable LUT**, which assumed that if the
        `PC Load Branch` signal was active the current instruction was a branch
        instruction, ignored its other inputs and set the `PC[L]` to load from
        the data bus (and thus `PC[H]` to load from the `DHPC`).
    *   There were two ways of fixing this that I could think of, one being a
        "trick" and the other being the "right way."
    *   The "trick" was to pretend that the `JSR` and `RTS` instructions had
        condition bits and to set these to `100`, meaning "branch never taken";
        this would prevent the `PC Load Branch` signal from being set, and would
        avoid the bug. However, it's conceptually unintuitive and questionable:
        firstly, since the `JSR` and `RTS` instructions are not branch
        instructions in that they are not conditional (and do not have condition
        bits); secondly, since incredibly, we'd need to set their "condition
        bits" to "never taken" to ensure that they would always be taken
        (correctly), since these act as special unconditional jumps!
    *   The "right way" was to instead change the 
        **AR Data Bus Load Enable LUT** implementation by letting the *loadAR*
        control signal override the `PC Load Branch` signal; i.e., if *loadAR*
        is set, then `PC Load Branch` should be ignored, since a branch
        instruction will not set it in the same cycle that it is branching.

Okay, now that this milestone has been reached, there is much to do before
ByteFrost Version 2 can be considered complete. To that end, this document
outlines the tasks that still need to be done to reach this goal, broken up by
category.

The task categories are:
1.  Hardware
2.  Software - Assembler
3.  Software - Filesystem
4.  Software - ByteFrost Programs
5.  Software / Infrastructure - Remote ByteFrost Interaction
6.  Documentation

From this task list, we can devise a roadmap towards ByteFrost Version 2.

##  Hardware

There are a few remaining hardware tasks.

### "Small" Hardware Updates

1.  Add the `BP` address register.
    *   This register can be implemented with two regular 8-bit registers.
    *   **Note:** As of January 18, 2026, this has been complete for a few 
        weeks.
2.  Replace the current `SP[H]` 8-bit register with two 4-bit counters.
    *   This will allow an arbitrarily large stack in RAM (instead of the 
        current stack that is limited to a single 256-byte page).

### Device Updates

The main software target of the Version 2 is the creation of a simple shell that
will allow the user to interact with the disk (via the filesystem) and run
programs from disk as well.

To that end, two main goals must be reached:
1.  Keyboard support
    *   Required to get user input for the shell.
    *   May or may not require interrupt support; potentially feasible to
        implement using polling only.
2.  Display upgrade
    *   Current display is a character display with 4 rows with 20 columns each.
        This was fine for smaller programs, but for a shell it is incredibly
        limiting.
    *   That said, the display can remain as a character display; I see no
        reason to have a pixel-based display, especially given the performance
        constraints of the ByteFrost (e.g., how long would it take to update a
        frame buffer?)

One promising approach to handle both of these is to add tty support. This could
allow using a laptop to connect to the ByteFrost and act as both the display
output and the keyboard input of the ByteFrost (both via tty). Hence, we'd just
need a single tty MMIO interface for this to work. This interface would need to
take into account the needs of the shell and solve the problem of handling the
input character stream, potentially without interrupts, and will likely require
flow control if the input stream is sent at faster than keyboard-typing rate
(may need to enforce some sort of rate-limiting).

However, if extended, this could allow remote access to the ByteFrost! Assuming
it is running and that we can remotely connect to the laptop that's connected to
its tty interface, one could remotely open the ByteFrost shell and interact with
it! That would be something.

##  Software - Assembler

### Derived Instructions

1.  First - verify that the entire ByteFrost ISA is correctly implemented in the
    assembler and that every ISA instruction has a corresponding assembly
    instruction (or multiple, as in the case of the `ALU` and `Branch Absolute`
    instructions).
2.  Add a derived instruction to set an AR in its entirety:
    1.  `LDA ARDest, 16-bit Immediate`
        *   E.g., `LDA %SP, #0x6502`
        *   This could be implemented using two `LDA` ISA instructions.
    2.  `MGA ARDest, {Rs1, Rs2}`
        *   E.g., `MGA %DP, {R1, R0}`
        *   This could be implemented using two `MGA` ISA instructions.
        *   **Note:** Need to figure out the syntax for such an instruction; the
            one I proposed here is likely not optimal.

### New Features

Need to think carefully about the assembler syntax here!

This also prompts a potential need for a generalization regarding handling
addresses in the assembler and specifying addresses explicitly.

1.  Add character constants.
    *   This is a fairly old request, but at present, to print characters using
        `OUT` instructions, one either needs to memorize the exact byte
        representation of the character or use local `.define` directives. This
        pollutes the assembly file with preprocessor constant definitions that
        should be either built-in, or more likely, be represented using a
        different token type entirely.
    *   Example: 
        *   Instead of
            ```asm
            .define 1 _A    0x41 
            //  ...
            OUT _A, A
            ```
        *   Support:
            ```asm
            OUT 'A' 
            ```
            *   **Note:** Note the lack of the A operand to specify character
                mode! This will likely be an overloaded instruction with a
                different list of expected tokens, e.g., just one:
                `TokenType::Character`
2.  Add data regions.
    *   It is fairly typical to want to store data as part of a binary file
        along with code; this allows storing static data directly, like strings,
        arrays, variables, etc.
    *   Example:
        ```asm
        .data [data]
        ```
        *   **Note:** This uses preprocessor directive syntax, but I'm not sure
            that that is the most appropriate approach; may need to change this.
        *   Data could be represented as an array of bytes like
            *   `.data [0x01, 0x02, 0x03, 0x04, -124, 'a', '\0', +83]`
        *   Data could also be represented as a string array (of bytes) like
            *   `.string "Hello World"`
                *   Such a data region would include a `\0` (0-byte)
                    null-terminator at the end.
        *   Length of the data region can be determined by the assembler so it
            need not be explicitly included, unlike for preprocessor constants.
3.  Generalize address handling in the assembler.
    *   At present, one can specify addresses in the assembler using two
        methods:
        1.  Labels (e.g., `:main`)
            *   Main purpose is to act as addresses to instructions (i.e.,
                instruction addresses)
        2.  Preprocessor constants (e.g., `.define 2 device1 0xE200`)
            *   Main purpose is to act as general static addresses (i.e.,
                byte addresses)
    *   However, labels cannot be assigned an explicit value (their value is
        implicit based on the number of ISA instructions preceding them and
        whatever the start address is (which can be set using the `.start`
        preprocessor directive))
    *   The previously mentioned data regions would add yet another way to
        specify addresses. E.g., it seems to make perfect sense to do something
        like
        ```asm
        :msg_string
        .string "Hello World"
        
        //  Set DP to point at beginning of msg_string
        LDA %DP, :msg_string

        //  Can now iterate through the static string
        //  ...
        ```
    *   This is based on some x86 assembly I've seen and seems quite elegant.
        Unfortunately, the question now becomes how to discern addresses set as
        labels to point at instructions (2 bytes) versus to point at a byte.
    *   I think the approach could be to always use labels by default as byte
        addresses and to only shift them left by 1 either:
        1.  When used as operands for certain instructions that use them as
            instruction addresses (i.e., branch instructions and `CALL`)
        2.  Have a different prefix character for "raw byte address" of the
            label than for the "instruction address" of the label
            *   E.g.,
                ```asm
                //  Label definition remains the same
                :main   //  E.g., 0x2000 byte address = 0x1000 instruction
                        //  address
                
                //  Label definition that is explicitly set to an address
                //  Here, it's set to a byte address. Does it ever make sense to
                //  set a label to an instruction address? I don't think so,
                //  again assuming that all labels / address constants use
                //  byte addresses by default and are only shifted in certain
                //  use cases explicitly? Or implicitly by depending on the
                //  instruction?
                :func = 0x8000

                //  Specify to set DP to instruction address value of :func
                //  (i.e., 0x4000) since JSR will jump to it.
                //  This is ugly, and can be replaced with CALL anyway. Perhaps
                //  this can be an issue for function pointers, but perhaps that
                //  can also be avoided by ensuring that function pointers are
                //  ALWAYS instruction addresses anyway (as they'll need to be
                //  since JSR does not shift the value of %DP before writing it
                //  to the %PC)?
                LDA %DP, I:func
                JSR

                ```
    *   Hmm, I'm not exactly sure. I want this to be as simple as possible for
        the programmer. I wonder how MIPS / ARM / etc. assemblers handle this,
        since their PC is likely shifted as well.
4.  Import / Export label system
    *   As the ByteFrost becomes more complex, the number locations of where 
        machine code can be grows.
    *   At present, there are three possible locations:
        1.  In ROM.
        2.  In disk.
        3.  In RAM (copied from ROM or from disk).
    *   Of course, when jumping to a label, the label address is within the
        ByteFrost address space and thus can point to ROM or RAM (technically
        also to MMIO regions, though the assembler should never allow a label
        to be set to an address after RAM).
    *   However, there are two potential issues with this:
        1.  If the label to jump to is not known by the assembler (since it
            isn't hardcoded to the assembler nor defined in the given assembly
            file), the assembler with throw an error.
            *   This is a compile-time problem.
        2.  If the label is known but that address in memory does not contain
            the relevant code, the ByteFrost will fetch garbage at that address.
            *   This is a run-time problem.
            *   If the label's address is in ROM, then this assumingly shouldn't
                occur if the ROM's EEPROM contains the correct binary file.
                If this occurs, it cannot be fixed in runtime - rather, it
                represents a hardware configuration fix (the EEPROM must be
                replaced or reflashed with the correct ROM binary).
    1.  Labels Unknown to Assembler (Compile-Time, Static)
    2.  Needed Code Not Loaded Yet (Run-Time, Dynamic)
        
##  Software - Filesystem

The current ByteFrost Filesystem V1 is useful as a proof of concept, but I want
to expand it significantly.

Current limitations:
1.  Only one root directory (flat directory structure).
2.  Only supports 16 files
3.  Disk size capped at `512 KB`
4.  File size is capped at `31.5 KB` 

Each disk sector's size is `256` bytes, and currently the sector ID is `11` bits
meaning that in total the disk address is `11 + log_2(256) = 11 + 8 = 19` bits
wide, giving us `512 KB`.

As I understand it, without changing the hardware (but with some changes to the
disk driver), we can increase the sector ID from `11` bits to `15` bits, giving
us `15 + 8 = 23` bit address or `8 MB`, which is excellent and I'm assuming will
be more than enough moving forward (but we can always change the hardware
interface to give us another sector ID byte; that would gives us another `8`
bits or `23 + 8 = 31`, with gets us a whopping `2 GB` maximum, which is more 
than enough for the ByteFrost).

I want to create a new and improved ByteFrost filesystem which has the following
features:
1.  One root directory with multiple sub-directories that can be created and
    deleted.
2.  Effectively arbitrary number of files that can be supported, though there
    may be a limit per-directory depending on the size of the directory blocks.
    (I don't think directory blocks need to necessarily fit in a sector, though;
    they could be made to use multiple contiguous sectors, e.g. 2 or 4, unless
    there's some fundamental issue with this that I'm not seeing).
3.  Disk size at least `8 MB`.
    *   If we use a disk larger than `16 MB` (16-bit sector IDs), we'd need to
        support 3-byte block pointers, which may pose additional difficulties.
4.  File size cap should be higher than `31.5 KB` if possible.
    *   In the original implementation, inodes only have direct data block
        pointers. We can introduce another layer of indirection to increase the
        file size limit.
5.  Symbolic links
6.  Hard links
7.  May want to tweak the size of boot code? It's presently `2 KB` which may be
    too much / too little.

Essentially, a more general-purpose UNIX-like filesystem.

May want to review File systems from CSE 120 and OS: 3 Easy Pieces for this.

##  Software - ByteFrost Programs

1.  Complete BIST
    *   Covers entire ISA and checks full instruction semantics.
2.  Update bootloader, if necessary.
    *   Will likely need to create an updated bootloader for FileSystem v2 but
        it may take a while to design and implement the new filesystem.
3.  Port old programs to new ISA updates.
    1.  Eight Queens solver.
    2.  Binary search tree implementation.
4.  Manually-compiled C programs.
    1.  Examples from Addressing Modes Proposal (v2) can be modified slightly to
        do this.
5.  BASIC interpreter? (or just have a BASIC cross-compiler instead).
6.  ByteFrost Shell
    *   The key goal for Version 2! Should support:
        1.  Keyboard input handling + printing to screen (tty)
        2.  Running commands using direct execution model (load them to known
            address then `JSR` to that address (should eventually return when
            returning from `main()`))
            *   Need to create the full programming model for this to work and
                the stack calling convention is vital for this too since all
                programs called by the shell should use it so that the shell
                correctly receives control back.
        3.  Built-in commands
            *   E.g., `cd`, `mkdir`, `ls`, etc. (anything that requires the
                current working directory to work properly, but ideally this
                list should be small to avoid making the shell too big).
        4.  External command programs (stored in `/bin` in the new filesystem)
            *   Can be almost any BASH utility (though of course with some
                limitations).
                *   Can have `exec` for programs that want the full RAM to
                    themselves, overwriting the shell (i.e., copy the program
                    into the shell at `0x2000` instead of after it, but this
                    is tricky - need to avoid overwriting the code to copy while
                    doing this! May require a copy primitive to be stored in 
                    ROM).
            *   May want to have support for scripting / BASIC(?)
        5.  Pass command-line arguments to the program that is run.
            *   I.e., provide argc, argv inputs to the `main()` of the loaded
                program.
        6.  Collects the status code of the run program.
            *   I.e., `main()` returns a status code to the shell.

##  Software / Infrastructure - Remote ByteFrost Interaction

The idea here is to provide remote access to the ByteFrost via an intermediary
(a modern Linux laptop):

Remote machine <-> Intermediary Linux laptop <-> ByteFrost

1.  Remote File Transfer Support
    *   Be able to update the disk file in the SD Card remotely.

##  Documentation

1.  Full hardware documentation
2.  Full ISA documentation
3.  Full device documentation (for MMIO interface use by programmers).
4.  Full Assembler (programmer's guide) documentation

##  Roadmap