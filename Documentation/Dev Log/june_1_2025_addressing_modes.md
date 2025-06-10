#   ByteFrost Development Log - Addressing Modes Proposal

June 1 - 4, 2025

##  Discussion

In this development log, we shall go over the new addressing modes proposal and
how it affects the ISA, control signals, and combinational logic hardware in the
ByteFrost. Hence, this log is divided into the following sections:
1.  Updates to the ISA
2.  Updates to Control Signals
3.  Updates to Combinational Logic

### 1.  Updates to the ISA

The new addressing mode instructions add a number of new instructions and
remove some instructions. Other instructions may now be marked as deprecated.

####    1.  New Instruction String Operands

There are `3` new instruction operands in this proposal. They are:

| Operand | Size (in bits) | Bit Locations | Semantics |
| ---     | ---            | ---           | ---       |
| `(AR) L/H` | 1 | 5 | Whether the high or low byte of an AR operand is used (relevant for writing or reading a data word from / to the data bus) |
| `ARSrc` | 2 | **Special Case; See Below** | Specify which AR to read from |
| `ARDest` | 2 | 7:6 | Specify which AR to write to |

**Note:** The `ARSrc` bit location depends on the opcode of the current
instruction in the following way:
1.  If the opcode is of the new `MAG` instruction, then `ARSrc` is in bits 
    `9:8`.
2.  Otherwise, `ARSrc` is in bits `5` and `0` (the lsb of the opcode).
*   In both cases, the higher bit is the msb of `ARSrc`. (`9,8` or `5,0`).

#####   New Instruction Operand Values

1.  `(AR) L/H`

| Bit 0 (location: `5`) | Meaning |
| ---                   | ---     |
| `0` | Low Byte of an AR |
| `1` | High Byte of an AR |

2. `ARSrc` and `ARDest`

| Bit 1 (location: `ARSrc`: `9` or `5`; `ARDest`: `7`) | Bit 0 (location: `ARSrc`: `8` or `0`; `ARDest`: `6`) | Meaning |
| ---                          | ---                          | ---     |
| `0` | `0` | `PC` (Note: `PC` means DHPC and PC Low; PC High is NOT ISA-addressable!) |
| `0` | `1` | `DP` |
| `1` | `0` | `SP` |
| `1` | `1` | `BP` |

####    2.  New Instructions

There are `6` new instructions, which require a total of `9` opcodes. They are:

**Note:** The opcode assignment of the new instructions assumes that the `LSP`
instruction is removed and that none of the deprecated instructions are removed.
Different opcode assignments may be required / more optimal if these assumptions 
are false.

| Instruction | Semantics | Example Usage | Number of Opcodes | Opcode Assignment |
| ---         | ---       | ---           | ---               | --- |
| `MAG` | `Rd = ARSrc[L/H]` | `MAG R1, %SP[H]` | 1 | `0x1a` |
| `LDW` | `Rd = *(ARSrc + Imm)` | `LDW R2, 16(%BP)` | 2 | `0x14` and `0x15` |
| `SDW` | `*(ARSrc + Imm) = Rs` | `SDW R3, -4(%DP)` | 2 | `0x16` and `0x17` |
| `LDA` | `ARDest[L/H] = Imm` | `LDA %DP[L], #0x54` | 1 | `0x1b` |
| `MGA` | `ARDest[L/H] = Rs1` | `MGA %BP[L], R2` | 1 | `0x1c` |
| `MAA` | `ARDest = ARSrc + Imm` | `MAA %SP, %SP, #-1` | 2 | `0x18` and `0x19` |

The new instructions have the following instruction strings:

1.  `MAG Rd, ARSrc[L/H]` (`Rd = ARSrc[L/H]`)

| `15:10` | `9:8` | `7:6` | `5` | `4:0` |
| ---     | ---   | ---   | --- | ---   |
| X | `ARSrc` | `Rd` | `(AR) L/H` | Opcode |

2.  `LDW Rd, Imm(ARSrc)` (`Rd = *(ARSrc + Imm)`)

| `15:8` | `7:6` | `5` | `4:0` |
| ---    | ---   | --- | ---   |
| `Imm_8` | `Rd` | `ARSrc` | Opcode (and `ARSrc`) |

3.  `SDW Rs, Imm(ARSrc)` (`*(ARSrc + Imm = Rs)`)

| `15:8` | `7:6` | `5` | `4:0` |
| ---    | ---   | --- | ---   |
| `Imm_8` | `Rd` | `ARSrc` | Opcode (and `ARSrc`) |

4.  `LDA ARDest[L/H], #Imm` (`ARDest[L/H] = Imm`)

| `15:8` | `7:6` | `5` | `4:0` |
| ---    | ---   | --- | ---   |
| `Imm_8` | `ARDest` | `(AR) L/H` | Opcode |

5.  `MGA ARDest[L/H], Rs1` (`ARDest[L/H] = Rs1`)

| `15:14` | `13:12` | `11:8` | `7:6` | `5` | `4:0` |
| ---     | ---     | ---    | ---   | --- | ---   |
| X | `Rs1` | X | `ARDest` | `(AR) L/H` | Opcode |

6.  `MAA ARDest, ARSrc, #Imm` (`ARDest = ARSrc + Imm`)

| `15:8` | `7:6` | `5` | `4:0` |
| ---    | ---   | --- | ---   |
| `Imm_8` | `ARDest` | `ARSrc` | Opcode (and `ARSrc`) |

####    3.  Removed Instructions

There is `1` instruction that should be removed, which frees up `1` opcode. It 
is:

| Instruction | Semantics | Example Usage | Number of Opcodes | Opcode Assignment |
| ---         | ---       | ---           | ---               | --- |
| `LSP` | `ARDest[H] = Imm` (where `ARDest` != `BP`) | `LSP %HDP, #0x30` | 1 | `0x14` |

The `LSP` instruction should be removed because it is eclipsed by the proposed
`LDA` instruction, which is a generalization of it.

####    4.  Deprecated Instructions and Instruction Operands

The following `11` instructions, in their present form, can be marked as
deprecated. `7` may be removed at present, `2` may not be removed but need to be
updated, and `2` may be removed in the future.

| Instruction | Semantics | Example Usage | Number of Opcodes | Opcode Assignment | Remove? |
| ---         | ---       | ---           | ---               | --- | --- |
| Branch Relative | If the condition bits `C2C1C0` match the ALU flag register bits, then `PC[L] = PC[L] + Immediate` | `BEQ +14` | 1 | `0x07` | Yes |
| `LMA` | `Rd = *({DP[H], Imm}), DP[L] = Imm` | `LMA R2, #0x05` | 1 | `0x09` | Yes |
| `SMA` | `*({DP[H], Imm}) = Rs, DP[L] = Imm` | `SMA R3, #0x30` | 1 | `0x0a` | Yes |
| `LMR` | `Rd = *({DP[H], Rs1}), DP[L] = Rs1` | `LMR R2, R1` | 1 | `0x0b` | Yes |
| `SMR` | `*({DP[H], Rs1}) = Rs, DP[L] = Rs1` | `SMR R1, R3` | 1 | `0x0c` | Yes |
| `PUSH` | `*(SP) = Rs1, SP++` | `PUSH R0` | 1 | `0x0e` | Need update / remove |
| `POP` | `SP--, Rd = *(SP)` | `POP R2` | 1 | `0x0f` | Need update / remove |
| `JSR` | `*(SP) = PC[L], SP++, PC[H] = DHPC, PC[L] = Imm` | `JSR #0x05` | 1 | `0x10` | Need update, don't remove |
| `RTS` | `SP -= 2, DHPC = *(SP), SP++, PC[L] = *(SP), SP--, PC]H] = DHPC` | `RTS` | 1 | `0x11` | Need update, don't remove |
| `OUT` | `Print Rs1 as an ASCII character or hex integer` | `OUT R2, A` | 1 | `0x08` | Remove in future |
| `OUT immediate` | `Print Immediate as an ASCII character or hex integer` | `OUT #0x54, I` | 1 | `0x0d` | Remove in future |

1.  Branch Relative (opcode `0x07`) is marked as deprecated since it only
    updates the low byte of the program counter; i.e., it can only branch within
    an "instruction page" (i.e., the 512 byte region starting with 9 lsbs being
    0 and ending with 9 lsbs being 1). Additionally, the ByteFrost Assembler v2
    only uses absolute branches, so this instruction isn't currently used in
    ByteFrost Assembly.
2.  Instructions `LMA`, `SMA`, `LMR`, and `SMR` (opcodes `0x09` to `0x0c`) may 
    be removed since they have unexpected semantics (they overwrite the Low byte
    of the `DP` address register) and are effectively replaced by `LDW` and 
    `SDW` (with `LDA` and `MGA` to set address register bytes with an 
    immediate / GR).
3.  Instructions `PUSH` and `POP` (opcodes `0x0e` and `0x0f`) implement an
    empty-ascending stack, whereas the new stack will be full-descending.
4.  `JSR` and `RTS` (opcodes `0x10` and `0x11`) are a bit more complicated - 
    they cannot be removed, but their semantics must be updated.
*   `JSR` is problematic for two reasons:
    1.  It only pushes the low byte of the return address onto the stack.
    2.  It is implemented for an empty-ascending stack.
    *   We can change the microcode of `JSR` using the current architecture to
        be for a full-descending stack, but we cannot change it so that it
        pushes the entire PC onto the stack. The reason for this is that it
        would require writing a byte of the PC to the Data Bus and at the same
        time write the SP to the Address Bus; this is only possible to do for
        `PC[L]` due to the dedicated *PC Out* control signal which writes 
        `PC[L]` to the Data Bus directly. To write `PC[H]` to the Data Bus, it
        must first be written to the Address Bus, which at present will cause a
        structural hazard.
    *   The pseudo-microcode of `JSR` that pushes only `PC[L]` to a
        full-descending stack is as follows:
        1.  **Cycle 1.**    Decrement SP.
        *   *Stack Pointer Count* = 1
        *   *Stack Pointer Increment / Decrement* = 0
        2.  **Cycle 2.**    Write PC[L] to the stack.
        *   Write PC[L] to the Data Bus using the *PC Out* (= 1) control signal.
        *   Set Memory to read from the Data Bus (*Mem Write* = 1).
        3.  **Cycle 3.**    Jump to immediate address.
        *   Write Immediate to the Data Bus.
        *   Set PC[L] to read from the Data Bus.
    *   Then the `CALL` Assembly instruction is built as follows: (overwrites
        `R0`)
        1.  `LDR R0, PC_HI`
        2.  `PUSH R0`
        3.  `DHPC (PC[H] using ARDest[H]) = JMP_ADDR_HI`
        4.  `JSR ADDR_LOW`
*   `RTS` is problematic because it is implemented for an empty-ascending stack.
    *   The pseudo-microcode of `RTS` that sets the `PC` to the top two bytes on
        the stack for a full-descending stack is as follows:
        (Assume that the `SP` points at `ret[L]` and `SP + 1` points at `ret[H]`
        (little-endian)):
        1.  **Cycle 1.**    Increment stack pointer. (Now `SP` points at 
            `ret[H]`).
        2.  **Cycle 2.**    Set `DHPC` to the data word at the top of the stack.
        3.  **Cycle 3.**    Decrement stack pointer. (Now `SP` points at 
            `ret[L]`) .
        4.  **Cycle 4.**    Set `PC[L]` to the data word at the top of the stack
            and increment stack pointer (now `SP` points at `ret[H]`).
        5.  **Cycle 5.**    Increment stack pointer (now `SP` points at the byte
            above `ret[H]`) and advance PC.
5.  `OUT` and `OUT immediate` (opcodes `0x08` and `0x0d`) may eventually be 
    deprecated, along with the instruction operand `A/I (Display)`:
    *   Eventually, it is advisable to remove the `OUT` instructions and move 
        the display interface to an MMIO device.
    *   This will save `2` opcodes, and allow for a more general display
        interface. It will also save on some hardware that is presently in use 
        (such as an OUT display register to save the data word (representing a 
        character / integer) on the data bus so that it may be available for 
        reading for a longer period of time by the Arduino Nano display
        controller).
    *   However, as no MMIO display interface presently exists, the `OUT`
        instructions cannot be deprecated.

### 2.  Updates to Control Signals

**TODO: FINISH THIS SECTION**

The new addressing mode instructions require adding / renaming control signals.
Other control signals may be removed.

####    1.  New / Renamed Control Signals

The new addressing mode instructions require adding `5` new control signals and
renaming `1` control signal:

| Control Signal | Signal ID Assignment | Instructions that use it | New or Renamed? |
| --- | --- | --- | --- |
| *loadAR* | `17` | `LDA`, `MGA` | Renaming of *Load Special Register* |
| *AddressBusToDataBus* | `18` | `MAG` | New |
| *AddressHorL* | `19` | | New |
| *TmpARRead* | `20` | `LDW` | New |
| *TmpARWrite* | `21` | `SDW` | New |
| *loadARHorL* | `22` | `MAA` | New |

####    2.  Removed Control Signals

The new addressing mode instructions allow for the removal of `?` control
signals:

### 3.  Updates to Combinational Logic

The new addressing mode instructions require adding / modifying / removing some
combinational logic.

####    1.  New Combinational Logic

There are `4` pieces of new combinational logic in the addressing modes 
proposal. They are:

1.  Decode Combinational Logic for the `ARSrc` Operand
2.  AR Data Bus Load Enable
*   A combinational logic circuit that sends load enable signals to AR byte
    registers to load a data word from the Data Bus.
3.  ARSelect
*   A combinational logic circuit that sends output enable signals to ARs to
    write to the Address Bus.
4.  AddressByteSelect
*   A combinational logic circuit that selects which byte of the Address Bus
    (H or L) to write to the Data Bus (this selection is logically ANDed with
    the *AddressBusToDataBus* control signal so that the byte is only written if
    specified).

#####   1.  Decode Combinational Logic for the `ARSrc` Operand

**Inputs:** Opcode (`INSTR[4:0]`), `INSTR[5]`, `INSTR[9:8]`

**Outputs:** `ARSrc` (2 bits).

**Logic:**
1.  If `INSTR[4:0] == MAG_OPCODE`:
    1.  `ARSrc = INSTR[9:8]`.
2.  Else:
    1.  `ARSrc = {INSTR[5], INSTR[0]}`.



####    2.  Updated Combinational Logic

####    3.  Removed Combinational Logic

**TODO:**   Add programming examples with new ISA instructions + change `JSR` to
    be `JSR (%DP)` so that it pushes the return address to the stack and jumps
    to the address in the `DP` (allows function pointers); can create `CALL`
    assembly instruction with `CALL :label` to add instructions that set the
    DP to the `:label` address
    Or maybe `JSR Imm` which jumps to `{DP[H], Imm}` (and sets `DP[L] = Imm`)?
    Or maybe `JSR Rs` which jumps to `{DP[H], Rs}` (and sets `DP[L] = Rs`)?