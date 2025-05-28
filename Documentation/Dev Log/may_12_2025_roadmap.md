#   ByteFrost Development Log - Current Roadmap

May 12 - 16, 2025

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

*This is largely a recap / restatement of the discussion from the two previous
development logs regarding Addressing Modes.*

The ByteFrost v2 framework has two different word types:
1.  Data word (8 bits)
    *   Stored in data registers (i.e., general-purpose registers (`R0 - R3`)
        and some intermediate / ISA non-accessible registers)
    *   Can be specified as an immediate in some instructions
        (e.g., `LDR R2, #255`)
    *   Passed via the Data Bus
2.  Address word (16 bits)
    *   Stored in special (address) registers (i.e., Data Pointer `DP`, Stack
        Pointer `SP`, and Program Counter `PC`*)
        *   The `PC` is special in that it is shifted left by 1 bit so that it
            points at instructions and not bytes. This is because instructions
            are 16-bits long, and effectively creates two kinds of addresses -
            general (16-bit) addresses of bytes, and instruction (shifted
            16-bit) addresses (hence, to set the `PC` to a "general" address
            such as `0x2000`, for instance, it must be set to instruction
            address `0x1000`).
            *   To avoid confusion, the assembler should be the only entity that
                needs to handle instruction addresses in code directly, and the
                programmer can always safely use general addresses without
                worrying about this irregularity of the program counter with
                respect to the other address registers.
    *   Passed via the Address Bus (whole), but could also be passed in halves
        (low or high bytes) through the Data Bus

As there are two word types, there are two register types (and two main busses
    for transferring the two word types):
1.  Data Registers
    1.  General Purpose Registers (`R0 - R3`) (Register File registers)
        *   ISA-accessible / -addressable
    2.  Other ISA non-accessible data registers (used by hardware implementation
        only)
2.  Address Registers
    1.  Data Pointer (`DP`), Stack Pointer (`SP`), and Program Counter (`PC`)*
        *   These are presently called Special Registers (SRs), but Address
            Registers (ARs) seems to be a more appropriate term for their usage
            and purpose.
    2.  Other ISA non-accessible address registers

Let the set of the forms of memory accessible via the ByteFrost v2 ISA be

$$
    M = \{\text{Immediate}, \text{GR}, \text{AR}, \text{Memory}, \text{Stack}\}
$$

There are therefore five distinct forms of ISA-accessible memory in the
ByteFrost v2 ISA:
1.  Immediate (R)
    *   Immeidate values specified in instruction strings.
    *   Data word size (8 bits, some variants 4 bits but are zero-extended
        like in `ALU immediate`)
2.  General Purpose Register ($\text{GR}$) (R/W)
    *   `R0 - R3`
    *   Data word size (8 bits)
3.  Address Register ($\text{AR}$) (R/W)
    *   `DP`, `SP`, `PC`
    *   Address word size (16 bits)
4.  Byte-Addressable Memory ($\text{Memory}$) (R/W)
    *   16-bit address space (64KB) (ROM, RAM, MMIO regions)
    *   Data word size (8 bits)
5.  Byte-Addressable Stack ($\text{Stack}$) (R/W)
    *   Dynamic sub-region of RAM.
    *   Full-Descending* Stack.
        *   Full: The `SP` points to the last byte on the stack.
        *   Descending: Pushing to the stack decrements the `SP`, popping from
            the stack increments the `SP`.
        *   **Note:** At present, the ByteFrost stack is Empty-Increasing, but
            this should be changed to a descending stack since the ByteFrost is
            little-endian (in principle, these concepts aren't related, but
            since the `CALL` assembly instruction pushes the high byte of the
            return address before `JSR` pushes the low byte of the return
            address, using an increasing stack will store the return address in
            a big-endian fashion, whereas a descending stack will store the
            return address in little-endian fashion, which is more consistent).
    *   Start address: arbitrary (though earliest address is top of the RAM,
        i.e., `1101 1111 1111 1111` = `0xDFFF`)
    *   End address: address pointed to by `SP` (if the `SP` address becomes
        smaller than the highest address of the next dynamic region in memory,
        i.e., the heap / code regions, stack overflow has occurred)
    *   Data word size (8 bits)
    *   Technically, the Stack is a part of Memory, so writing to Memory ca
        also be writing to the Stack, and writing to the Stack always involves
        writing to Memory (the same applies for reading). However, the Stack
        involves different, more specific operations (`PUSH` and `POP`), and
        thus meirts its distinction from Memory as another "form of memory" that
        is addressable by the ByteFrost v2 ISA.

Let $G = (M, E)$ be a graph such that

$$
\begin{aligned}
E = \{(u, v) \in M \times M ~|~ \exists \text{ an instruction in the ByteFrost v2 ISA that allows} \\
\text{writing from memory form } u \text{ to memory form } v\}
\end{aligned}
$$

This means that $E \subseteq M \times M$;
$|M \times M| = |M| \cdot |M| = 5 \times 5 = 25$, so there are at most $25$
edges in this graph, each of which could represent at least one instruction
(though not necessarily unique instructions, assuming some instructions could
exist that allow writing from / to multiple forms of memory). But not all edges
make sense (for instance, writing to an immediate; hence, we only need to
seriously consider 20 of the edges).

Since the ByteFrost uses a load-store architecture, for maximal expressive
power, we only need that reachability between any node to any writable node is
guaranteed via the $\text{GR}$ node (i.e., the graph can minimally be a start
graph with the $\text{GR}$ node being the center; since the graph is
directional, this will require $5 \times 2 = 10$ edges, or about 10 instructions
(exactly 10 assuming no overlap between the instructions exists)).

If a path exists from node $u$ to $v$ and $(u, v) \notin E$, then adding an
instruction to the ISA so that $(u, v) \in E$ improves code brevity /
performance but not expressive power.

In the following, we consider the graph $G$ and discuss each possible edge
(ignoring edges that represent writing to $\text{Immediate}$). Since an edge
$(u, v)$ represents a way to write from memory form $u$ to memory form $v$, we
will represent $(u, v)$ perhaps more intuitively as $u \leftarrow v$.

1.  $\text{Immediate} \rightarrow \text{GR}$
    *   `Rd = Imm`
    *   Useful? Yes
    *   Change Required? No
    *   Dedicated Instruction(s)? Yes (edge exists in $G$)
        *   `LDR`
    *   Implementation in code:
        *   `LDR Rd, #Imm`
2.  $\text{GR} \rightarrow \text{GR}$
    *   `Rd = Rs`
    *   Useful? Yes
    *   Change Required? No
    *   Dedicated Instruction(s)? Yes (edge exists in $G$)
        *   `MOV`
    *   Implementation in code:
        *   `MOV Rd, Rs`
3.  $\text{AR} \rightarrow \text{GR}$
    *   `Rd = AR[i][L/H]`
    *   Useful? Yes
    *   Change Required? Yes
    *   Dedicated Instruction(s)? Need (edge doesn't currently exist in $G$)
4.  $\text{Memory} \rightarrow \text{GR}$
    *   `Rd = *({DP[H], Imm})`  (`LMA`)
    *   `Rd = *({DP[H], Rs})`   (`LMR`)
    *   `Rd = *(AR[i] + Imm)`   (Need new instruction for this)
    *   Useful? Yes
    *   Change Required? Yes
    *   Dedicated Instruction(s)? Yes (edge exists in $G$)
        *   `LMA, LMR`
        *   Need new instruction for `Rd = *(AR[i] + Imm)` semantics
5.  $\text{Stack} \rightarrow \text{GR}$
    *   `Rd = *(SP), SP++`
    *   Useful? Yes
    *   Change Required? Yes
    *   Dedicated Instruction(s)? Yes
        *   `POP` (but need to update `POP` to have the semantics of a full
            descending stack)
6.  $\text{Immediate} \rightarrow \text{AR}$
    *   `AR[i][L/H] = Imm`
    *   Useful? Yes
    *   Change Required? Yes
    *   Dedicated Instruction(s)? Yesn't
        *   `LSP` does this partially, but it needs to be updated to have the
            full semantics of this operation (at present, `LSP` only performs
            the partial semantics `AR[i][H] = Imm`)
7.  $\text{GR} \rightarrow \text{AR}$
    *   `AR[i][L/H] = Rs`
    *   Useful? Yes
    *   Change Required? Yes
    *   Dedicated Instruction(s)? Need (edge doesn't currently exist in $G$)
8.  $\text{AR} \rightarrow \text{AR}$
    *   `AR[i] = AR[j] + Imm`
    *   Useful? Yes
    *   Change Required? Yes
    *   Dedicated Instruction(s)? Need (edge doesn't currently exist in $G$)
    *   **Notes:**
        *   This instruction could be used to implement `PUSH` and `POP`
            operations in assembly; e.g.
            *   `PUSH Rs` has the semantics `SP--, *(SP) = Rs` and therefore
                can be implemented as
                *   `SP = SP + (-1)` (this instruction), followed by
                *   `*(SP) = Rs` ($\text{GR} \rightarrow \text{Memory}$ 
                    instruction)
            *   `POP Rd` has the semantics `Rd = *(SP), SP++` and therefore can
                be implemented as
                *   `Rd = *(SP)` ($\text{Memory} \rightarrow $\text{GR}$
                    instruction)
                *   `SP = SP + 1` (this instruction)
        *   Assuming the immediate used here is 8 bits, there aren't enough bits
            in an instruction string to store all of the instruction operands:
            *   Opcode - 5 bits
            *   Immediate - 8 bits
            *   `i` and `j` indices: 2 bits each (4 bits total)
            *   5 + 8 + 4 = 17 bits
            *   Possible solutions:
                1.  7 bit immediate.
                    *   This means that the immediate must be either zero or
                        sign-extended, and reduces its range by half (from
                        `0 - 255` or `-128 - 127` to `0 - 127` or `-64 - 63`)
                2.  Either `i` or `j` is reduced to 1 bit.
                    1.  `i` is reduced to 1 bit.
                        *   This means that this instruction can only be used to
                            set 2 out of the 3 (4 possible) ARs.
                        *   E.g., can set the `SP` and `DP`, but not `PC` (and
                            possibly `BP` base pointer if we add that as the
                            fourth AR). However, we can set the `SP` and `DP` to
                            the value of any of the 4 ARs.
                            *   This allows for `SP = SP +/- 1` for instance.
                    2.  `j` is reduced to 1 bit.
                        *   This means that this instruction can only be used to
                            set any AR to 2 out of the 3 (4 possible) ARs.
                        *   E.g., can set any `AR` to `SP` and `DP`, but not to
                            the `PC` (and possibly `BP` base pointer if we add
                            that as the fourth AR). However, we can use this
                            instruction to set any AR value to the 2 chosen ARs.
9.  $\text{Memory} \rightarrow \text{AR}$
    *   `AR[i][L/H] = *(AR[j] + Imm)` or
    *   `AR[i] = {*(AR[j + 1]), *(AR[j])}`
    *   Useful? Maybe
        *   Even so, since we use a load-store architecture, it would be more
            correct to store `*(AR[j] + Imm)` or `*(AR[j + 1])` and `*(AR[j])`
            in a GR (i.e., `Rd`) and then write that GR's value to address
            register `AR[i]` (specifying high or low byte when writing from the
            GR to `AR[i]`).
10. $\text{Stack} \rightarrow \text{AR}$
    *   `AR[i][L/H] = *(SP), SP++` (a sort of `POP AR[i][L/H]`), or
    *   `AR[i] = {*(SP + 1), *(SP)}, SP += 2` (a sort of `POP AR[i]`)
    *   Useful? Maybe
        *   Even so, since we use a load-store architecture, it would be more
            correct to store `*(SP)` (and perhaps `*(SP + 1)`) in a GR and then
            write that GR's value to `AR[i]` (specifying the high or low byte
            when writing from the GR to `AR[i]`).
11. $\text{Immediate} \rightarrow \text{Memory}$
    *   `*(AR[i]) = Imm`
        *   **Note:** This isn't `*(AR[i] + Imm) = Imm` since there isn't room
            for two 8-bit immediates in an instruction string.
    *   Useful? Yes
        *   E.g., for `PUSH Imm`
    *   Change Required? Likely not
    *   Dedicated Instruction(s)? Likely not
        *   Again, since we use a load-store architecture, it would be more
            correct to store the immediate in a GR and then write that GR's
            value at the address specified by `AR[i]` (using a
            $\text{GR} \rightarrow \text{Memory}$ instruction).
12. $\text{GR} \rightarrow \text{Memory}$
    *   `*({DP[H], Imm}) = Rs`  (`SMA`)
    *   `*({DP[H], Rs1}) = Rs2` (`SMR`)
    *   `*(AR[i] + Imm) = Rs`   (Need new instruction for this)
    *   Useful? Yes
    *   Change Required? Yes
    *   Dedicate Instruction(s)? Yes (edge exists in $G$)
        *   `SMA`, `SMR`
        *   Need new instruction for `*(AR[i] + Imm) = Rs` semantics
13. $\text{AR} \rightarrow \text{Memory}$
    *   `*(AR[i] + Imm) = AR[j][L], *(AR[i] + Imm + 1) = AR[j][H]`
    *   Useful? Maybe
        *   Again, since we use a load-store architecture, it would make more
            sense to write the values of `AR[j][L]` and `AR[j][H]` to GRs and
            then use a $\text{GR} \rightarrow \text{Memory}$ instruction to
            write the GR values to the addresses specified by `AR[i] + Imm` and
            `AR[i] + Imm + 1`.
14. $\text{Memory} \rightarrow \text{Memory}$
    *   `*(AR[i] + Imm) = *(AR[j])`
    *   Useful? Yes
    *   Change Required? Likely not
    *   Dedicated Instruction(s)? No
        *   The ByteFrost is a load-store architecture, ergo doing something
            like this requires reading the data word (1 byte) at `*(AR[j])`,
            storing it in a GR, and then storing the data word in that GR at
            `*(AR[i] + Imm)`.
15. $\text{Stack} \rightarrow \text{Memory}$
    *   `*(AR[i] + Imm) = *(SP), (and maybe SP++)`
    *   Useful? Maybe
        *   The ByteFrost is a load-store architecture, so this instruction
            doesn't make a lot of sense to add, even if it were useful.
16. $\text{Immediate} \rightarrow \text{Stack}$
    *   `*(SP) = Imm, SP--` (i.e., a `PUSH immediate`)
    *   Useful? Likely
    *   Change Required? Perhaps
    *   Dedicated Instruction(s)? Perhaps
        *   This might be a useful instruction to add, but it can be achieved
            via setting a GR to an immediate and then using `PUSH Rs`; hence,
            adding this instruction is purely a performance / code brevity
            optimization.
17. $\text{GR} \rightarrow \text{Stack}$
    *   `*(SP) = Rs, SP--`  (i.e., a `PUSH Rs`)
    *   Useful? Yes
    *   Change Required? Yes
    *   Dedicated Instruction(s)? Yes
        *   `PUSH Rs`
            *   However, `PUSH Rs` needs to be updated to support full
                descending stack semantics
        *   Can also be implemented using
            *   `*(SP) = Rs` (using a $\text{GR} \rightarrow \text{Memory}$
                instruction)
            *   `SP = SP + (-1)` (using a $\text{AR} \rightarrow \text{AR}$
                instruction)
18. $\text{AR} \rightarrow \text{Stack}$
    *   `*(SP) = AR[i][H], SP--, *(SP) = AR[i][L], SP--` (i.e., a `PUSH AR[i]`)
    *   Useful? Yes
    *   Change Required? Possibly
    *   Dedicated Instruction(s)? Possibly
        *   This could be a useful instruction for pushing address words to the
            stack quickly, but again can also be implemented by first storing
            each half of an AR in a GR and pushing the GR to the stack using
            `PUSH Rs`.
19. $\text{Memory} \rightarrow \text{Stack}$
    *   `*(SP) = *(AR[i] + Imm), SP--` (i.e., a `PUSH *(AR[i] + Imm)`)
    *   Useful? Possibly
    *   Change Required? Likely not
    *   Dedicated Instruction(s)? No
        *   Again, the ByteFrost uses a load-store architecture, so this should
            be implemented by first reading the data word `*(AR[i] + Imm)` and
            storing it in a GR, then pushing the GR to the stack using 
            `PUSH Rs`.
20. $\text{Stack} \rightarrow \text{Stack}$
    *   I'm not even sure what the semantics of such an instruction would look
        like.

Now that we've gone over the possible edges and discussed their possible
semantics and dedicated instructions, here are the edges that should be added
(or whose current implementation needs to be changed / augmented) to $G$:

1.  (3) $\text{AR} \rightarrow \text{GR}$
    *   Semantics:  `Rd = AR[i][L/H]`   (New instruction)
2.  (4) $\text{Memory} \rightarrow \text{GR}$
    *   Semantics:
        *   `Rd = *({DP[H], Imm})`  (`LMA`)
            *   Note: Sets `DP[L]` to the given immediate!
        *   `Rd = *({DP[H], Rs})`   (`LMR`)
            *   Note: Sets `DP[L]` to the given register value!
        *   `Rd = *(AR[i] + Imm)`   (New instruction)
3.  (5) $\text{Stack} \rightarrow \text{GR}$
    *   Semantics: `Rd = *(SP), SP++`   (Augment `POP` to have these semantics)
        *   Need to update `POP` to implement a full descending stack
4.  (6) $\text{Immediate} \rightarrow \text{AR}$
    *   Semantics:  `AR[i][L/H] = Imm`  (Augment `LSP` to have these semantics)
        *   Need to allow `LSP` to set low byte of ARs
5.  (7) $\text{GR} \rightarrow \text{AR}$
    *   Semantics:  `AR[i][L/H] = Rs`   (New instruction)
6.  (8) $\text{AR} \rightarrow \text{AR}$
    *   Semantics: `AR[i] = AR[j] + Imm`    (New instruction)
    *   **Note:** Need 17 bits to specify all operands in instruction string
        assuming 8 bit immediate, but only have 16; need to choose from which
        operand to remove a bit.
7.  (12) $\text{GR} \rightarrow \text{Memory}$
    *   Semantics:
        *   `*({DP[H], Imm}) = Rs`  (`SMA`)
            *   Note: Sets `DP[L]` to the given immediate! 
        *   `*({DP[H], Rs1}) = Rs2` (`SMR`)
            *   Note: Sets `DP[L]` to the given register value!
        *   `*(AR[i] + Imm) = Rs`   (New instruction)
8.  (17) $\text{GR} \rightarrow \text{Stack}$
    *   Semantics: `*(SP) = Rs, SP--`   (Augment `PUSH` to have these semantics)
        *   Need to update `PUSH` to implement a full-descending stack

Hence, 5 new instructions need to be added, and 3 existing instructions need to
be modified / augmented.

In terms of priorities however, we can focus on 6 instructions first (ignoring
the updates to `PUSH` and `POP` to implement a full-descending stack).

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