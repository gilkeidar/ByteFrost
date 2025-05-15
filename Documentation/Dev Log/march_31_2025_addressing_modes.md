#   ByteFrost Development Log - On Addressing Modes (continued)

March 31, 2025

##  Discussion

We continue the discussion on addressing modes from the previous development log
on March 6, 2025.

First, to recap from the previous discussion:

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
            16-bit) addresses (hence, to set the PC to a "general" address such
            as `0x2000`, for instance, it must be set to instruction address
            `0x1000`).
            *   To avoid confusion, the assembler is the only entity that needs
                to handle instruction addresses in code directly, and the
                programmer can always safely use general addresses without
                worrying about this irregularity of the program counter with
                respect to the other address registers.
    *   Passed via the Address Bus (whole), but could also be passed in halves
        (low or high bytes) through the Data Bus

As there are two word types, there are two register types:
1.  Data Registers
    1.  General Purpose Registers (`R0 - R3`) (Register File registers)
    2.  Other ISA non-accessible data registers
2.  Address Registers
    1.  Data Pointer (`DP`), Stack Pointer (`SP`), and Program Counter (`PC`)*
        *   These are presently called Special Registers (SRs), but Address
            Registers (ARs) seems more appropriate for their usage and purpose.
    2.  Other ISA non-accessible address registers

Let the set of the forms of memory accessible via the ByteFrost v2 ISA be

$$
    M = \{\text{Immediate}, \text{GR}, \text{AR}, \text{Memory}, \text{Stack}\}
$$

There are therefore five distinct forms of ISA-accessible memory in the
ByteFrost v2 ISA:
1.  Immediate (R)
    *   Immediate values specified in instruction strings.
    *   Data word size (8 bits, some variants 4 bits but are zero-extended
        like in `ALU immediate`)
2.  General Purpose Register ($\text{GR}$) (R/W)
    *   `R0 - R3`
    *   Data word size (8 bits)
3.  Address Register ($\text{AR}$) (R/W)
    *   `DP`, `SP`, `PC`
    *   Address word size (16 bitgs)
4.  Byte-Addressable Memory ($\text{Memory}$) (R/W)
    *   16-bit address space (64 KB) (ROM, RAM, MMIO regions)
    *   Data word size (8 bits)
5.  Byte-Addressable Stack ($\text{Stack}$) (R/W)
    *   Dynamic sub-region of RAM.
    *   Full-Descending* Stack.
        *   Full: The `SP` points to the last byte on the stack.
        *   Descending: Pushing to the stack decrements the `SP`, popping from
            the stack increments the `SP`.
        *   **Note:** At present, the ByteFrost stack is Empty-Increasing, but
            this should be changed to a descending stack since the ByteFrost is
            little-endian (in principle these concepts aren't related, but since
            the `CALL` assembly instruction pushes the high byte of the return
            address before `JSR` pushes the low byte of the return address,
            using an increasing stack will store the return address in a
            big-endian fashion, whereas a descending stack will store the return
            address in a little-endian fashion, which is more consistent).
    *   Start address: arbitrary (though earliest address is top of the RAM,
        i.e., `1101 1111 1111 1111` = `0xDFFF`)
    *   End address: address pointed to by `SP` (if the `SP` address becomes
        smaller than the highest address of the next dynamic region in memory,
        i.e., the heap / code regions, stack overflow has occurred)
    *   Data word size (8 bits)
    *   Technically, the Stack is a part of Memory, so writing to Memory can
        also be writing to the Stack, and writing to the Stack always involves
        writing to Memory (the same applies for reading). However, the Stack
        involves different,, more specific operations (`PUSH` and `POP`), and
        thus merits its distinction from Memory as another "form of memory" that
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
make senes (for instance, writing to an immediate; hence we only need to
seriously consider 20 of the edges).

Since the ByteFrost uses a load-store architecture, for maximal expressive
power, we only need that reachability between any node to any writable node is
guaranteed via the $\text{GR}$ node (i.e., the graph can minimally be a star
graph with the $\text{GR}$ node being the center).

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
        *   `LSP` does this partially but needs to be updated to have the full
            semantics of this operation
7.  


