#   ByteFrost Development Log - On Addressing Modes

March 6, 2025

##  Discussion

In the ByteFrost v2 framework, we have two different word types which have
different sizes:
1.  Data word (8 bits)
    *   Stored in data registers (e.g., general-purpose registers (`R0 - R3`)
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
            general (16-bit) addresses of bytes and instruction (shifted 
            16-bit) addresses (hence, to set the PC to "general" address 
            `0x2000`, for instance, it must be set to instruction address 
            `0x1000`). To avoid confusion, the assembler is the only entity that
            needs to handle instruction addresses in code directly, and the
            programmer can always safely use general addresses without worrying
            about this irregularity of the program counter with respect to the
            other address registers.
    *   Passed via the Address Bus (whole), but could also be passed in halves
        (low or high bytes) through the Data Bus

To handle the two word types, there are therefore two register types:
1.  Data Registers
    1.  General Purpose Registers (`R0 - R3`) (Register File registers)
    2.  Other ISA non-accessible data registers
2.  Address Registers
    1.  Data Pointer (`DP`), Stack Pointer (`SP`), and Program Counter (`PC`)*
        *   These are presently called Special Registers (SRs), but Address
            Registers (ARs) seems more appropriate for their usage and purpose.
    2.  Other ISA non-accessible address registers

As such, there are three (technically four) forms of ISA-accessible memory:

0.  Immediate (Read-Only)
    *   Immediates specified in instruction strings.
    *   Data word size (8 bit, some variants 4 bits (e.g., `ALU immediate`)).
1.  General Purpose Register (GR) (R/W)
    *   `R0 - R3`
    *   Data word size (8 bit)
2.  Address Register (AR) (R/W)
    *   `DP`, `SP`, `PC`
    *   Address word size (16 bit)
3.  Byte-Addressable Memory (M) (R/W)
    *   16-bit (64KB) address space (ROM, RAM, MMIO regions)
    *   Data word size (8 bit)

For general use, each form of memory should be Read/Write-able using a value
from any other form of memory. I.e., something like
    $$M_i \leftarrow M_j \text{ where } M_i, M_j 
        \in \{\text{Imm}, \text{GR}, \text{AR}, \text{M}\}$$ 

Of course, not all combinations here make sense, (e.g., writing to an 
immediate), but this can help us see which combinations are possible. In total,
there are $|M \times M | = |M| \times |M| = 4 \times 4 = 16$ combinations, of
which we already ruled out four (writing to an immediate), leaving us with 12
remaining. Let us consider these:
1.  $\text{GR} \leftarrow \text{Imm}$
    *   `Rd = Imm_8`
    *   Useful? Yes
    *   Dedicated Instruction(s)?   Yes
        *   `LDR`
    *   Implementation in code:
        *   `LDR Rd, #Imm`
2.  $\text{GR} \leftarrow \text{GR}$
    *   `Rd = Rs`
    *   Useful? Yes
    *   Dedicated Instruction(s)?   Yes
        *   `MOV`
    *   Implementation in code:
        *   `MOV Rd, Rs`
3.  $\text{GR} \leftarrow \text{AR}$
    *   `Rd = AR[i][L / H]`
    *   Useful? Yes
    *   Dedicated Instruction(s)?   Yes (**Note: not yet implemented**)
        *   `MVA Rd, %H/LAR` (e.g., `MVA R2, %LDP`)
    *   Implementation in code:
        *   `MVA Rd, %H/LAR`
4.  $\text{GR} \leftarrow \text{M}$
    *   `Rd = *(AR[i] + Imm_8)`
    *   Useful? Yes
    *   Dedicated Instruction(s)?   Yes (**Note: not yet implemented**)
        *   `LMR Rd, Imm(AR[i])`    (e.g., `LMR R3, 5(%SP)` = `R3 = *(SP + 5)`)
    *   Implementation in code:
        *   `LMR Rd, Imm(AR[i])`
5.  $\text{AR} \leftarrow \text{Imm}$
    *   `AR[i][L / H] = Imm_8`
    *   Useful? Yes
    *   Dedicated Instruction(s)?   Likely Yes (**Note: partially implemented**)
        *   `LAR %H/LAR, #Imm`  (e.g., `LAR %HSP, #0x54` = `SP[H] = 0x54`)
    *   Implementation in code:
        *   `LAR %H/LAR, #Imm`
        *   `LDR Rd, #Imm` then `LAR %H/LAR, Rs`
6.  $\text{AR} \leftarrow \text{GR}$
    *   `AR[i][L / H] = Rs`
    *   Useful? Yes
    *   Dedicated Instruction(s)?   Yes (**Note: not yet implemented**)
        *   `LAR %H/LAR, Rs`
    *   Implementation in code:
        *   `LAR %H/LAR, #Imm`
7.  $\text{AR} \leftarrow \text{AR}$
    *   `AR[i] = AR[j] + Imm_8`
    *   Useful? Yes
    *   Dedicated Instruction(s)?   Yes (**Note: not yet implemented**)
        *   `LAR %AR, Imm(%AR)`   (e.g., `LAR %SP, 5(%SP)` = `SP = SP + 5`)
        *   **Note:** Can be used for `PUSH` and `POP`
            *   E.g., `PUSH x` can be done via two instructions that do
            *   `*(SP + 0) = x`
            *   `SP = SP - 1`   (`LAR %SP, -1(%SP)`)
    *   Implementation in code:
        *   `LAR %AR, Imm(%AR)`
8.  $\text{AR} \leftarrow \text{M}$
    *   `AR[i][L / H] = *(AR[j] + Imm_8)`
    *   Useful? Maybe...?
        *   Even if so, if we use a load-store architecture, it would be more
            correct to store `*(AR[j] + Imm_8)` in a GR (i.e., `Rd`) and then
            writign that GR's value to the address register.
9.  $\text{M} \leftarrow \text{Imm}$
    *   `*(AR[i]) = Imm_8`
        *   **Note:** This isn't `*(AR[i] + Imm_8) = Imm_8` since there isn't
            room for two immediates in na instruction string.
    *   Useful? Yes
        *   E.g., for `PUSH Imm`
    *   Dedicated Instruction(s)?   If possible
        *   Again, very useful for the `PUSH Imm` instruction, which is good so
            that the `CALL` instruction can be implemented in assembly without
            overwriting `R0` to save the return address.
        *   However, perhaps the `CALL` instruction could be implemented as part
            of the ISA by pushing the PC to memory...?
        *   `SMI #Imm, %AR` (e.g., `SMI #5, %SP` = `*SP = 5`)
    *   Implementation in code:
        *   `SMI #Imm, %AR`
        *   `LDR Rd, #Imm` then `SMR Rs, Imm(%AR)`
10. $\text{M} \leftarrow \text{GR}$
    *   `*(AR[i] + Imm_8) = Rs`
    *   Useful? Yes
    *   Dedicated Instruction(s)?   Yes (**Note: Not yet implemented**)
        *   `SMR Rs, Imm(%AR)`  (e.g., `SMR R2, 32(%SP)` = `*(SP + 32) = R2`)
    *   Implementation in code:
        *   `SMR Rs, Imm(%AR)`
11. $\text{M} \leftarrow \text{AR}$
    *   `*(AR[i] + Imm_8) = AR[j]`
        *   **Note:** This is abusing notation; technically, the semantics here
            are: (little-endian)
            1.  `*(AR[i] + Imm_8) = AR[j][L]`
            2.  `*(AR[i] + Imm_8 + 1) = AR[j][H]`
    *   Useful? Maybe...?
        *   Yes at least for pushing the `PC` to the stack to save the return
            address (may be useful for context switching also)
        *   This can be done using multiple instructions as follows:
            (This can be done in 4 instructions by swapping the last two for
            `SMR R0, (Imm + 1)(%AR)` = `*(AR[i] + Imm_8 + 1) = R0 = SP[H]`)
            1.  `MVA R0, %LSP` = `R0 = SP[L]`
            2.  `SMR R0, Imm(%AR)` = `*(AR[i] + Imm_8) = R0 = SP[L]`
            3.  `MVA R0, %HSP` = `R0 = SP[H]`
            4.  `LAR %AR, 1(%AR)` = `AR[i] = AR[i] + 1`
            5.  `SMR R0, Imm(%AR)` = `*(AR[i] + Imm_8 + 1) = R0 = SP[H]`
        *   Since the ByteFrost is a load-store architecture, it makes more
            sense to do this using multiple instructions as shown above rather
            than provide explicit instructions.
            *   Need to consider effect on `JSR` and `RTS` (since this is an
                example where pushing / popping an address register (AR) 
                to / from the stack (i.e., to memory) is necessarily done in a 
                single instruction)
12. $\text{M} \leftarrow \text{M}$
    *   `*(AR[i] + Imm_8) = *(AR[j])`
    *   Useful? Yes
    *   Dedicated Instruction(s)? No
        *   The ByteFrost is a load-store architecture, ergo doing something
            like this required reading the data word (1 byte) at `*(AR[j])`,
            storing it in a GR, then storing the GR to `*(AR[i] + Imm_8)`.



