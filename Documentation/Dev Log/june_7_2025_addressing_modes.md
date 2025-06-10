#   ByteFrost Development Log - Addressing Modes Proposal (v2)

June 7 - 9, 2025

##  Overview

In this development log, we shall go over the new addressing modes proposal. We
shall first go over the new instructions, their semantics, and example usage,
with motivations for the new instructions and how they improve the ISA (with
comparisons to how equivalent operations would be done in the current ISA). We
shall then discuss any instructions that can be removed or deprecated. Lastly,
we shall cover the required hardware changes in terms of updates to control
signal assignments and new combinational logic circuits.

##  ISA Updates

### New Instruction Semantics

There are `6` new instructions and `2` instructions with updated semantics;
their names and semantics are shown below.

| Instruction | Semantics | Example Usage | New or Modified? |
| ---         | ---       | ---           | ---              |
| `JSR` | `Push PC to the stack (SP--; *SP = PC[H]; SP--; *SP = PC[L]), jump to DP (PC = DP)` | `JSR` | Modified |
| `RTS` | `Pop return address from the stack (PC[L] = *SP; SP++; PC[H] = *SP; SP++;)` | `RTS` | Modified |
| `MAG` | `Rd = ARSrc[L/H]` | `MAG R1, %SP[H]` | New |
| `LDW` | `Rd = *(ARSrc + Imm)` | `LDW R2, 16(%BP)` | New |
| `SDW` | `*(ARSrc + Imm) = Rs` | `SDW R3, -4(%DP)` | New |
| `LDA` | `ARDest[L/H] = Imm` | `LDA %DP[L], #0x54` | New |
| `MGA` | `ARDest[L/H] = Rs1` | `MGA %BP[L], R2` | New |
| `MAA` | `ARDest = ARSrc + Imm` | `MAA %SP, %SP, #-1` | New |

####    `JSR` Discussion

In the proposal, the `JSR` instruction's operands and semantics are modified.

#####   `JSR`'s current semantics

In the current ISA, the `JSR` instruction has one immediate operand (`JSR Imm`)
and the following semantics: 
1.  Push the low byte of the `PC` to the stack.
    1.  `*SP = PC[L]` (Push `PC[L]` to the stack)
    2.  `SP++`
2.  Set the `PC` to the target address.
    1.  `PC[L] = Imm` (Set `PC[L]` to the low byte of the target address)
    2.  `PC[H] = DHPC` (Set `PC[H]` to `DHPC` which should contain the high byte
        of the target address)

This has the following issues:
1.  `JSR` doesn't push `PC[H]` to the stack, meaning it must be done by a prior
    instruction.
2.  `JSR` doesn't allow specifying the high byte of the target address, meaning
    it must be done by a prior instruction.
3.  `JSR` sets `PC[L]` to an immediate (i.e., function pointers are not 
    possible).
4.  `JSR` is implemented for an empty-ascending stack instead of a 
    full-descending stack.

Currently, the `CALL :label` assembly instruction is implemented in the 
following way:
1.  Push `PC[H]` to the stack.
2.  Set `DHPC` to the high byte of the target address.
3.  `JSR low_byte_of_target_address`.

```asm
//  1.  Push PC[H] to the stack.
LDR R0, #PC_HI_VALUE (known to assembler at compile time)
PUSH R0

//  2. Set DHPC to the high byte of the target address.
LSP %DHPC, #TARGET_ADDRESS_HI

//  3. JSR
JSR #TARGET_ADDRESS_LO
```

As we can see, the `CALL` assembly instruction is implemented using `4` ISA
instructions and has additional unwanted semantics, such as overwriting `R0`
(due to the lack of a `PUSH Imm` ISA instruction).

#####   `JSR`'s new semantics

In the proposal, `JSR` has the following semantics: 
1.  Push the `PC` to the stack.
    1.  `SP--`
    2.  `*SP = PC[H]`
    3.  `SP--`
    4.  `*SP = PC[L]`
2.  Jump to the address in `DP`.
    1.  `PC = DP`

This implementation resolves issues `1`, `3`, and `4`, but doesn't fix issue 
    `2`.
1.  **Issue 1:** `JSR` doesn't push `PC[H]` to the stack, meaning it must be 
    done by a prior instruction.
*   **Resolution:** `JSR` now pushes the entire `PC` to the stack, including 
    `PC[H]`, so that it doesn't need to be done by a prior instruction.
2.  **Issue 2:** `JSR` doesn't allow specifying the high byte of the target 
    address, meaning it must be done by a prior instruction.
*   **Worse:** `JSR` now doesn't allow specifying the high nor low byte of the
    target address, meaning they must both be set by prior instructions. The
    target address is always assumed to be in the `DP` Address Register.
3.  **Issue 3:** `JSR` sets `PC[L]` to an immediate (i.e., function pointers are
    not possible).
*   **Resolution:** `JSR` sets the `PC` to `DP`, a register; hence, the target
    address may change during runtime (allowing for function pointer semantics).
4.  **Issue 4:** `JSR` is implemented for an empty-ascending stack instead of a 
    full-descending stack.
*   **Resolution:** `JSR` is now implemented for a full-descending stack.

**Regarding Issue 2:** We can observe that `JSR` now doesn't take any operands
for specifying the target address, and assumes that the target address is
already loaded into `DP`. That means that calling an arbitrary address takes
`3` instructions (`DP[L] = target_address_low`, `DP[H] = target_address_hi`, 
`JSR`). This is an improvement over the `4` instruction implementation of `CALL`
and has no other unusual semantics. `JSR` can be modified to have an additional
Immediate or GR operand to specify parts of the address, but the first option
restricts the function pointer semantics (issue `3`) and the second requires
loading the relevant target address byte to a GR. I think that the proposed
implementation is a clean way to handle this with a smaller extra instruction
cost than the current implementation.

Now, the `CALL :label` assembly instruction can be implemented as follows:
1.  Set `DP = TARGET_ADDRESS`.
2.  `JSR`.
```asm
//  1.  Set DP to the target address.
LDA %DP[L], #TARGET_ADDRESS_LOW
LDA %DP[H], #TARGET_ADDRESS_HI

//  2.  JSR.
JSR
```

####    `RTS` Discussion

In the proposal, the `RTS` instruction's operands and semantics are modified.

#####   `RTS`' current semantics

In the current ISA, the `RTS` instruction has the following semantics:
1.  Pop the return address from the stack and load it to the `PC`.
    1.  `SP--`                          (`SP` now points at `ret[L]`)
    2.  `SP--`                          (`SP` now points at `ret[H]`)
    3.  `DHPC = *SP, PC[H] = DHPC`      (`DHPC` is now set to `ret[H]`)
    4.  `SP++`                          (`SP` now points at `ret[L]`)
    5.  `PC[L] = *SP`                   (`PC[L]` is now set to `ret[L]`)
    6.  `SP--`                          (`SP` now points at `ret[H]` (empty byte
                                        after end of stack))

This has the following issue:
1.  `RTS` is implemented for an empty-ascending stack instead of a 
    full-descending stack.

#####   `RTS`' new semantics

In the proposal, `RTS` has the following semantics:
1.  Pop the return address from the stack and load it to the `PC`. (same)
    1.  `PC[L] = *SP`.              (`SP` points at `ret[L]` and `PC[L]` is set 
                                    to `ret[L]`)
    2.  `SP++`.                     (`SP` now points at `ret[H]`)
    3.  `DHPC = *SP, PC[H] = DHPC`. (`DHPC` is now set to `ret[H]`)
    4.  `SP++`.                     (`SP` now points at the byte above `ret[H]`
                                    (new last byte of the stack))

This implementation resolves issue `1`.
1.  **Issue 1:** `RTS` is implemented for an empty-ascending stack instead of a 
    full-descending stack.
*   **Resolution:** `RTS` is now implemented for a full-descending stack.

####    `MAG` Discussion

The proposal introduces the `MAG` instruction, which has the following 
semantics:
1.  `Rd = ARSrc[L/H]`.

This instruction provides the following benefits:
1.  **Issue 1:** In the current ISA, there is no way to write the value of an
    Address Register high / low byte to a General Purpose Register.
*   **Resolution:** This is now possible using the `MAG` instruction.

####    `LDW` Discussion

The proposal introduces the `LDW` instruction, which has the following
semantics:
1.  `Rd = *(ARSrc + Imm)`.

This instruction provides the following benefits:
1.  **Issue 1:** Handling 16-bit addresses is complex; the current ISA mitigates
    some of the complexity by setting the high byte of an address in the Special
    Registers `DP[H]`, `SP[H]`, and `DHPC`. Then, instructions such as `LMA`,
    `SMA`, `LMR`, `SMR`, and Branch Absolute set the bottom byte and perform 
    memory loads / stores / update the PC in one instruction. `LMA`, `SMA`, 
    `LMR`, and `SMR` can only use the `DP` to store the address and overwrite
    the `DP[L]` byte, however.
*   **Improvement:** Now handling addresses might be slightly simplified, since
    we can use dedicated Address Registers to hold addresses and avoid updating
    them when we have spatial (of +127/-128 byte offsets) or temporal locality 
    in accessing memory (specifically in terms of loads / stores). `LDW` is
    better than `LMA` and `LMR` since it allows using additional Address 
    Registers besides the `DP` and doesn't overwrite the `DP` low byte.
    *   The spatial locality benefit is the same as for `LMA` and `LMR` (256
        byte range), but it is now centered around the `DP` address instead of
        the page specified by `DP[H]`. This may be equivalent, better, or worse,
        depending on the situation. I think this is better for situations that
        require known offsets from a base address (e.g., accessing struct fields
        or offsets in a stack frame), since then the offset is the same every
        time, whereas the offset would change if the struct isn't stored at the
        beginning of a page (i.e., at some address `{DP[H], 00}`).

This instruction allows deprecating `LMA` and `LMR`.

####    `SDW` Discussion

The proposal introduces the `SDW` instruction, which has the following
semantics:
1.  `*(ARSrc + Imm) = Rs`.

This instruction provides the following benefits:
1.  **Issue 1:** The current memory store instructions (`SMA` and `SMR`) can
    only use the `DP[H]` as the high byte of the address, and overwrite `DP[L]`.
*   **Improvement:** `SDW` allows using any ISA-addressable AR as a base address
    (i.e., `PC`, `DP`, `SP`, or `BP`), does not overwrite the used AR, and
    maintains the same amount of spatial locality now centered at the address in
    the used AR (better for known offsets when the address is arbitrary, such as
    in struct field accesses).

This instruction allows deprecating `SMA` and `SMR`.

####    `LDA` Discussion

The proposal introduces the `LDA` instruction, which has the following
semantics:
1.  `ARDest[L/H] = Imm`.

This instruction provides the following benefits:
1.  **Issue 1:** The current ISA allows only setting the high byte of some ARs
    (`DHPC`, `DP[H]`, and `SP[H]`) to an immediate. There is no way to set the 
    low byte of an AR to an immediate (except for the unusual semantics of `LMA`
    and `SMA` that set `DP[L]` to their immediate operand).
*   **Resolution:** `LDA` allows setting any byte of an ISA-addressable AR
    (`DHPC`, `PC[L]`, `DP[H/L]`, `SP[H/L]`, and `BP[H/L]`) to an immediate.

This instruction allows removing `LSP`.

####    `MGA` Discussion

The proposal introduces the `MGA` instruction, which has the following
semantics:
1.  `ARDest[L/H] = Rs1`.

This instruction provides the following benefits:
1.  **Issue 1:** The current ISA does not allow setting any AR byte to the value
    of a GR.
*   **Resolution:** `MGA` allows setting any byte of an ISA-addressable AR
    (`DHPC`, `PC[L]`, `DP[H/L]`, `SP[H/L]`, and `BP[H/L]`) to a GR.

####    `MAA` Discussion

The proposal introduces the `MAA` instruction, which has the following
semantics:
1.  `ARDest = ARSrc + Imm`.

This instruction provides the following benefits:
1.  **Issue 1:** The current ISA does not allow setting an AR to another AR's
    value.
*   **Resolution:** `MAA` allows setting any ISA-addressable AR (`PC`, `DP`,
    `SP`, and `BP`) to another ISA-addressable AR (i.e., with 
    `ARDest = ARSrc + 0`)
2.  **Issue 2:** The current ISA does not provide an easy way for performing
    16-bit pointer arithmetic.
*   **Resolution:** `MAA` allows for limited 16-bit pointer arithmetic by
    allowing adding an immediate offset to an ISA-addressable AR's value (the
    limit being that the immediate is signed 8-bit (in the range 
    `[-128, 127]`)).
2.  **Issue 2:** The current ISA makes implementing an x86-style calling
    convention very difficult if not impossible. Some of the reasons for this 
    are that an additional AR (`BP`) is needed, setting `SP` to `BP` and vice 
    versa is needed, and accessing the stack from the `BP` with an offset is 
    needed to access variables (taken care of by `LDW` and `SDW`).
*   **Resolution:** The proposal introduces the new `BP` AR and `MAA` allows
    for easily setting one AR to another.

### Example Code

Here are a few example programs / program snippets.

####    1.  Stack Calling Convention

Let us attempt to implement the following calling convention (copied largely
from the 32-bit x86 calling convention, see
https://aaronbloomfield.github.io/pdr/book/x86-32bit-ccc-chapter.pdf):

1.  Caller (Before Callee Runs)
    1.  Push caller-saved registers (`R0` - `R3`).
    *   **Note:** ARs are NOT saved. The `SP` and `BP` are always for the
        current function's stack frame, the `PC` points at the next instruction,
        and the `DP` is overridable; the caller should assume its value is not
        preserved after a call to a function.
    2.  Push callee arguments (last in the order first)
    3.  Push return address and jump to callee.
2.  Callee (Prolog)
    *   The caller's return address is at the top of the stack.
    1.  Push the current `BP` (save the caller's base pointer).
    2.  Set the `BP` to the `SP` (defines the callee's base pointer).
    3.  Make space for local variables by decrementing the `SP` by the total
        size of the callee's local variables in bytes.
3.  Callee (Epilog)
    0.  (Return Value) Set the `DP` to the address of the return value.
    1.  Free the stack frame by setting the `SP` to the `BP`.
    2.  Pop the caller's `BP` from the stack (i.e., set the `BP` to 
        `{*(SP + 1), *SP}`)
    3.  Pop the return address and return to the caller.
4.  Caller (After Callee Returns)
    1.  Pop the callee arguments from the stack.
    2.  Restore the caller-saved registers (`R0` - `R3`).
    3.  (Return Value) Use return value whose address is in DP.

There's a question of how to pass the return value to the caller; I think a
simple approach is to always return a pointer to it in the `DP`. (This may
present complications / not be optimal, however, since the return value may be
stored in the callee's stack frame, in which case the caller needs to make a
local copy first, which may need to be implemented carefully if the return value
is a type that's too large).

Let us attempt to do this for the following C code:
```c
uint8_t add(uint8_t x, uint8_t y) {
    uint8_t z = x + y;
    return z;
}

int main() {
    //  ...
    uint8_t a = add(3, 5);
    //  ...
}
```

This is almost impossible to implement in the current ISA.

Let us attempt to implement it using the new instructions:
```asm
:main
    //  ...
    //  uint8_t a = add(3, 5);
    //  1.  Push caller-saved registers
    PUSH R0
    PUSH R1
    PUSH R2
    PUSH R3
    
    //  2.  Push callee arguments
    LDR R0, #5
    PUSH R0     //  Push argument for uint8_t y
    LDR R0, #3
    PUSH R0     //  Push argument for uint8_t x

    //  3.  Push return address and jump to callee.
    LDA %DP[H], TARGET_ADDRESS_HI (:add[H])
    LDA %DP[L], TARGET_ADDRESS_LO (:add[L])
    JSR

    //  4.  Pop the callee arguments from the stack
    POP R0      //  Pop argument for uint8_t x
    POP R0      //  Pop argument for uint8_t y

    //  5.  Restore the caller-saved registers
    POP R3
    POP R2
    POP R1
    POP R0

    //  6.  Use return value (uint8_t a = RV = *DP); assuming that a is in R1
    LDW R1, 0(%DP)
    
    //  ...

:add
    //  Prolog
    //  1.  Push the current BP. (Save the caller's BP)
    MAG R0, %BP[H]      //  Push BP[H]
    PUSH R0
    MAG R0, %BP[L]      //  Push BP[L]
    PUSH R0

    //  2.  Set the BP to the SP.
    MAA %BP, %SP, #0

    //  3.  Make space for local variables (1 byte for uint8_t z)
    MAA %SP, %SP, ADD_LOCAL_VARS_OFFSET (#-1)

    //  z = x + y.
    LDW R0, 4(BP)   //  R0 = *(BP + 4) = x
    LDW R1, 5(BP)   //  R1 = *(BP + 5) = y
    ADD R2, R0, R1  //  R2 = R0 + R1 = x + y
    SDW R2, -1(BP)  //  *(BP - 1) = x + y --> z = x + y

    //  Epilog
    //  0.  (Return Value) Set the DP to the address of the return value.
    MAA %DP, %BP, #-1   //  DP = BP - 1 = &z

    //  1.  Free the stack frame by setting the SP to the BP.
    MAA %SP, %BP, #0    //  SP = BP + 0 = BP

    //  2.  Pop the caller's BP from the stack
    POP R0              //  R0 = *SP = callerBP_LO
    MGA %BP[L], R0      //  BP[L] = R0 = callerBP_LO
    POP R0              //  R0 = *SP = callerBP_HI
    MGA %BP[H], R0      //  BP[H] = R0

    //  3.  Pop the return address and return to the caller.
    RTS
```

####    2.  Iterating through an array of structs

Let us attempt to implement the following C code using the new proposal's
instructions:

```c
typedef struct {
    char * playerName;
    uint8_t health;
    uint8_t maxHealth;
    uint8_t isFighting;
} player_t Player;

int main() {
    //  Let Player * players point to an array of Player structs of length 125.
    Player * healthiestPlayer = players[0];
    for (uint8_t i = 1; i < 125; i++) {
        if (players[i]->health > healthiestPlayer->health) {
            healthiestPlayer = &(players[i]);
        }
    }

    return healthiestPlayer->health;
}
```

First, we can observe that the `Player` struct has size `2 + 1 + 1 + 1 = 5` 
bytes, with offsets `playerName_offset = 0`, `health_offset = 2`, 
`maxHealth_offset = 3`, and `isFighting_offset = 4`.

The assembly code for this could look as follows:
```asm
:main
    // Prolog
    //  1.  Save the caller's BP.
    MAG R0, %BP[H]      //  Push BP[H]
    PUSH R0
    MAG R0, %BP[L]      //  Push BP[L]
    PUSH R0

    //  2.  Set the BP to the SP.
    MAA %BP, %SP, #0

    //  3.  Make space for local variables
    //      sizeof(players) + sizeof(healthiestPlayer) + sizeof(i) = 
    //          = (125 * 5) + 2 + 1 (+ 2) = 630
    //      BP -->  BP[L]
    //              players[124][4]
    //              players[124][3]
    //              ...
    //              players[0][0]
    //              healthiestPlayer[H]
    //              healthiestPlayer[L]
    //              i
    //              DPTemp[H]
    //              DPTemp[L]
    //      Therefore, SP = SP - 630
    
    LDR R0, #4     //  R0 = 4
    :sp_lv_space_for_loop   //  (SP local variable making space for loop)
        TST R0, #0                      //  while (R0 > 0) 
        BEQ :sp_lv_space_for_loop_end   //  {
        MAA %SP, %SP, #-128             //      SP = SP - 128;
        DEC R0                          //      R0--;
        JMP :sp_lv_space_for_loop       //  }
    :sp_lv_space_for_loop_end
    MAA %SP, %SP, #-118                 //  SP = SP - 118

    //  Player * healthiestPlayer = players[0]
    MAA %DP, %SP, #5    //  DP = &(players[0])
    MAG R0, %DP[H]      //  R0 = (&(players[0]))[H]
    SDW R0, 4(%SP)      //  *(SP + 2) = healthiestPlayer[H] = &(players[0])[H]
    MAG R0, %DP[L]      //  R0 = (&(players[0]))[L]
    SDW R0, 3(%SP)      //  *(SP + 1) = healthiestPlayer[L] = &(players[0])[L]

    LDR R0, #1
    SDW R0, 2(%SP)      //  uint8_t i = 1;
    :for_loop_1
        TST R0, #125            //  for (; i < 125; i++)
        BEQ :for_loop_1_end     //  {
        
        //  if (players[i]->health > healthiestPlayer->health)
        MAA %DP, %DP, #11       //  DP = &(players[i - 1]) + sizeof(Player)
        LDW R1, health_offset(%DP)  //  R1 = player[i]->health
        
        //  Save DP in DPTemp (DPTemp = DP)
        MAG R2, %DP[H]          //  R2 = DP[H] = &players[i][H]
        SDW R2, 1(%SP)
        MAG R2, %DP[L]          //  R2 = DP[L] = &players[i][L]
        SDW R2, 0(%SP)

        //  DP = healthiestPlayer
        LDW R2, 4(%SP)          //  R2 = healthiestPlayer[H]
        MGA %DP[H], R2          //  DP[H] = healthiestPlayer[H]
        LDW R2, 3(%SP)          //  R2 = healthiestPlayer[L]
        MGA %DP[L], R2          //  DP[L] = healthiestPlayer[L]

        LDW R2, health_offset(%DP)  //  R2 = healthiestPlayer->health

        //  DP = DPTemp (&players[i])
        LDW R3, 1(%SP)          //  R3 = &players[i][H]
        MGA %DP[H], R3          //  DP[H] = &players[i][H]
        LDW R3, 0(%SP)          //  R3 = &players[i][L]
        MGA %DP[L], R3          //  DP[L] = &players[i][L]

        SUB R2, R2, R1          //  R2 = healthiestPlayer->health 
                                //      - player[i]->health
        BPL :after_if_1
            //  healthiestPlayer = &(players[i]);
            MAG R1, %DP[H]      //  R1 = &players[i][H]
            SDW R1, 4(%SP)      //  healthiestPlayer[H] = &players[i][H]
            MAG R1, %DP[L]      //  R1 = &players[i][L]
            SDW R1, 3(%SP)      //  healthiestPlayer[L] = &players[i][L]
        :after_if_1

        //  i++
        INC R0                  //  i++
        SDW R0, 2(%SP)
        JMP :for_loop_1         //  }
    :for_loop_1_end

    //  Epilog
    //  0.  (Return Value) Set the DP to the address of the return value
    //  return healthiestPlayer->health;
    //  Set DP = &(healthiestPlayer->health)
    LDW R0, 4(%SP)              //  R0 = healthiestPlayer[H]
    MGA %DP[H], R0              //  DP[H] = healthiestPlayer[H]
    LDW R0, 3(%SP)              //  R0 = healthiestPlayer[L]
    MGA %DP[L], R0              //  DP[L] = healthiestPlayer[L]
    MAA %DP, %DP, health_offset //  DP = healthiestPlayer + health_offset

    //  1.  Free the stack frame by setting the SP to the BP.
    MAA %SP, %BP, #0    //  SP = BP + 0 = BP

    //  2.  Pop the caller's BP from the stack
    POP R0              //  R0 = *SP = callerBP_LO
    MGA %BP[L], R0      //  BP[L] = R0 = callerBP_LO
    POP R0              //  R0 = *SP = callerBP_HI
    MGA %BP[H], R0      //  BP[H] = R0

    //  3.  Pop the return address and return to the caller.
    RTS
```

####    3.  Iterating through doubly linked list

### Instruction String Operands Updates

There are `3` new instruction operands in this proposal. They are:

| Operand | Size (in bits) | Bit Locations | Semantics |
| ---     | ---            | ---           | ---       |
| `(AR) L/H` | `1` | `5` | Whether the high or low byte of an AR operand is used (relevant for writing or reading a data word from / to the data bus) |
| `ARSrc` | `2` | **Special Case; See Below** | Specify which AR to read from |
| `ARDest` | `2` | `7:6` | Specify which AR to write to |

**Note:** The `ARSrc` bit location depends on the opcode of the current
instruction in the following way:
1.  If the opcode is of the new `MAG` instruction, then `ARSrc` is in bits 
    `9:8`.
2.  Otherwise, `ARSrc` is in bits `{5, 0}` (where `0` is the lsb of the opcode).
*   In both cases, the higher bit is the msb of `ARSrc` (`9` or `5`).

####    New Instruction Operand Values

1.  `(AR) L/H`

| Bit 0 (location: `5`) | Meaning |
| ---                          | ---     |
| `0` | Low Byte of an AR |
| `1` | High Byte of an AR |

2.  `ARSrc` and `ARDest`

| Bit 1 (location: `ARSrc`: `9` or `5`; `ARDest`: `7`) | Bit 0 (location: `ARSrc`: `8` or `0`; `ARDest`: `6`) | Meaning |
| --- | --- | --- |
| `0` | `0` | `PC` (Note: When `ARSrc` is `PC`, then `PC` is `{PC[H], PC[L]}`; when `ARDest` is `PC`, then `PC` is `{DHPC, PC[L]}`!) |
| `0` | `1` | `DP` |
| `1` | `0` | `SP` |
| `1` | `1` | `BP` |

### New Instructions

There are `6` new instructions, which require a total of `9` opcodes. They are:

**Note:** The opcode assignment of the new instructions assumes that the `LSP`
instruction is removed and that none of the deprecated instructions are removed.
Different opcode assignments may be required / more optimal if these assumptions
are false.

| Instruction | Semantics | Example Usage | Number of Opcodes | Opcode Assignment |
| ---         | ---       | ---           | ---               | ---               |
| `MAG` | `Rd = ARSrc[L/H]` | `MAG R1, %SP[H]` | `1` | `0x1a` |
| `LDW` | `Rd = *(ARSrc+ Imm)` | `LDW R2, 16(%BP)` | `2` | `0x14` and `0x15` |
| `SDW` | `*(ARSrc + Imm) = Rs` | `SDW R3, -4(%DP)` | `2` | `0x16` and `0x17` |
| `LDA` | `ARDest[L/H] = Imm` | `LDA %DP[L], #0x54` | `1` | `0x1b` | 
| `MGA` | `ARDest[L/H] = Rs1` | `MGA %BP[L], R2` | `1` | `0x1c` |
| `MAA` | `ARDest = ARSrc + Imm` | `MAA %SP, %SP, #-1` | `2` | `0x18` and `0x19` |

The new instructions have the following instruction strings:

1.  `MAG Rd, ARSrc[L/H]` (`Rd = ARSrc[L/H]`)

| `15:10` | `9:8` | `7:6` | `5` | `4:0` |
| ---     | ---   | ---   | --- | ---   |
| X | `ARSrc` | `Rd` | `(AR) L/H` | Opcode |

2.  `LDW Rd, Imm(ARSrc)` (`Rd = *(ARSrc + Imm)`)

| `15:8` | `7:6` | `5` | `4:0` |
| ---    | ---   | --- | ---   |
| `Imm_8` | `Rd` | `ARSrc` | Opcode (and `ARSrc` lsb) |

3.  `SDW Rs, Imm(ARSrc)` (`*(ARSrc + Imm = Rs)`)

| `15:8` | `7:6` | `5` | `4:0` |
| ---    | ---   | --- | ---   |
| `Imm_8` | `Rd` | `ARSrc` | Opcode (and `ARSrc` lsb) |

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
| `Imm_8` | `ARDest` | `ARSrc` | Opcode (and `ARSrc` lsb) |

### Updated Instructions

There are `4` updated instructions. They are:

| Instruction | Current Semantics | New Semantics | Example New Usage | Number of Opcodes | Opcode Assignment |
| --- | --- | --- | --- | --- | --- |
| `PUSH` | `*SP = Rs1, SP++` | `SP--, *SP = Rs1` | `PUSH R2` | `1` | `0x0e` |
| `POP` | `SP--, Rd = *SP` | `Rd = *SP, SP++` | `POP R2` | `1` | `0x0f` |
| `JSR` | `*SP = PC[L], SP++, PC[L] = Imm, PC[H] = DHPC` | `SP--, *SP = PC[H], SP--, *SP = PC[L], PC = DP` | `JSR` | `1` | `0x10` |
| `RTS` | `SP -= 2, DHPC = *SP, PC[H] = DHPC, SP++, PC[L] = *SP, SP--` | `PC[L] = *SP, SP++, DHPC = *SP, PC[H] = DHPC, SP++` | `RTS` | `1` | `0x11` |

`PUSH` and `POP` are updated to support a full-descending stack instead of an
empty-increasing one.

`JSR` and `RTS` are similarly updated, with `JSR` updated to push the full PC
(return address) to the stack and jump to the address in the `DP` (allowing for
function pointer semantics).

**Note:** `JSR`'s new implementation requires hardware support to push the `PC`
value to the stack; since AR values are written to the Data Bus via the address
bus, this requires having the `PC` L/H byte on the Data Bus while the `SP`'s 
value is on the Address Bus. To do this, writing a byte from the Address Bus to
the Data Bus must involve writing to a temporary register first, which will hold
the value and only write it to the Data Bus in the next cycle.

### Removed Instructions

There is `1` instruction that should be removed, which frees up `1` opcode. It
is:

| Instruction | Semantics | Example Usage | Number of Opcodes | Opcode Assignment |
| ---         | ---       | ---           | ---               | ---               |
| `LSP` | `ARDest[H] = Imm` (where `ARDest` != `BP`) | `LSP %DP, #0x30` | `1` | `0x14` |

The `LSP` instruction should be removed because it is eclipsed by the proposed
`LDA` instruction, which is a generalization of it.

### Deprecated Instructions and Instruction Operands

The following `5` instructions, in their present form, can be marked as 
deprecated, and eventually removed to free up `5` opcodes.

| Instruction | Semantics | Example Usage | Number of Opcodes | Opcode Assignment |
| ---         | ---       | ---           | ---               | ---               |
| Branch Relative | If the condition bits `C2C1C0` match the ALU flag register bits, then `PC[L] = PC[L] + Immediate` | `BEQ +14` | `1` | `0x07` |
| `LMA` | `Rd = *({DP[H], Imm}), DP[L] = Imm` | `LMA R2, #0x05` | `1` | `0x09` |
| `SMA` | `*({DP[H], Imm}) = Rs, DP[L] = Imm` | `SMA R3, #0x30` | `1` | `0x0a` |
| `LMR` | `Rd = *({DP[H], Rs1}), DP[L] = Rs1` | `LMR R2, R1` | `1` | `0x0b` |
| `SMR` | `*({DP[H], Rs1}) = Rs, DP[L] = Rs1` | `SMR R1, R3` | `1` | `0x0c` |

1.  Branch Relative (opcode `0x07`) may be marked as deprecated since it only
    updates the low byte of the `PC`; i.e., it can only branch within an
    "instruction page" (i.e., the 512 byte region starting with 9 lsbs being `0`
    and ending with 9 lsbs being `1`, with the high 7 bits unchanged).
    Additionally, the ByteFrost Assembler v2 only uses absolute branches, so
    this instruction isn't used by any ByteFrost Assembly program assembled with
    the ByteFrost Assembler v2.
2.  Instructions `LMA`, `SMA,` `LMR`, and `SMR` (opcodes `0x09` to `0x0c`) may
    be marked as deprecated since they have unexpected semantics (they overwrite
    the low byte of the `DP` address register) and are effectively deprecated by
    `LDW` and `SDW` (with `LDA` and `MGA` to set address register bytes with an
    immediate / GR).

####    Instructions that could be deprecated in the future

The following `2` instructions can be marked as deprecated in the future once
a set of conditions is cleared.

| Instruction | Semantics | Example Usage | Number of Opcodes | Opcode Assignment |
| ---         | ---       | ---           | ---               | ---               |
| `OUT` | `Print Rs1 as an ASCII character or hex integer` | `OUT R2, A` | `1` | `0x08` |
| `OUT Immediate` | `Print Immediate as an ASCII character of hex integer` | `OUT #0x54, I` | `1` | `0x0d` |

1.  `OUT` and `OUT Immediate` (opcodes `0x08` and `0x0d`, respectively) may
    eventually be deprecated, along with the instruction operand 
    `A/I (Display)`, when the following condition is cleared:
    1.  Replace the current display with a display whose interface to the
        ByteFrost is part of the MMIO section of the address space.
*   This will allow for the following benefits:
    1.  Free `2` opcodes.
    2.  Allow for a more general display interface, and potentially for a larger
        display (e.g., hopefully a `600x800` resolution?)

##  Control Signals Updates

### New Control Signals

The proposal introduces `5` new control signals. They are:

| Control Signal | Signal ID Assignment | Instructions that use it |
| ---            | ---                  | ---                      |
| *loadARHorL* | `18` | `?` |
| *TmpARRead*  | `19` | `?` |
| *TmpARWrite* | `20` | `?` |
| *AddressBusToDataBus* | `21` | `?` |
| *AddressHorL* | `22` | `?` |

**Note:** *loadARHorL* and *AddressHorL* match up in all the instructions that
    use them (or can; i.e., one might be set to a particular value and the other
    may be `X`; it is never the case that one is `1` and the other is `0` in the
    same cycle, for instance, and since they're both "guarded" by *loadAR* and
    *AddressBusToDataBus*, respectively, they're `X` when not used instead of
    `0`).

### Updated Control Signals

The proposal updates (renames) `2` control signals. They are:

| Control Signal | Old Name | Signal ID Assignment | Instructions that use it |
| ---            | ---      | ---                  | ---                      |
| *loadAR* | *Load Special Register* | `17` | `?` |
| *SP Out* | *RAM Address Select* | `14` | `?` |

### Removed Control Signals

`?`

##  Combinational Logic Updates

Implementing this proposal requires updating `5` of the combinational logic
units of the ByteFrost:
1.  Decode - `ARSrc` Operand
2.  AR Data Bus Load Enable (Replaces the Load Special Pointer unit)
3.  ARSelect (Replaces the Address Bus Arbiter)
4.  AddressByteSelect
5.  PC Loading (When `PC[L]` is loaded, set `PC[H]` to load from `DHPC`).

### 1.  Decode - `ARSrc` Operand

**Inputs:** Opcode (`INSTR[4:0]`), `INSTR[5]`, `INSTR[9:8]`

**Outputs:** `ARSrc` (2 bits).

**Logic:**
1.  If Opcode == `MAG_OPCODE`:
    1.  `ARSrc = INSTR[9:8]`.
2.  Else:
    1.  `ARSrc = {INSTR[5], INSTR[0]}`.

**Implementation Diagram:** **TODO**

### 2.  AR Data Bus Load Enable

**Inputs:** *loadAR* (control signal), *loadARHorL* (control signal), 
    Opcode (`INSTR[4:0]`), `(AR) L/H` instruction operand  (`INSTR[5]`),
    `ARDest` instruction operand (`INSTR[7:6]`)
    PC Load Low (Active Low)
**Outputs:** {`None (No AR reads from Data Bus)`, `DHPC Load Enable`, 
    `PC[L] Load Enable`, `DP[H] Load Enable`, `DP[L] Load Enable`, 
    `SP[H] Load Enable`, `SP[L] Load Enable`, `BP[H] Load Enable`,
    `BP[L] Load Enable`}

**Note:** Can't write to `PC[H]` from the Data Bus directly - `PC[H]` reads from
the `DHPC` whenever `PC[L]` loads! When PC Load Low is active, we have the
following semantics: `PC[L] = Data Bus, PC[H] = DHPC`. This means that when
writing to the PC, we must first write to the `DHPC` and then to `PC[L]`.

**Logic:**
1.  Identify AR to write to.
2.  Identify AR Byte to write to.

1.  If PC Load Low is active (active low):
    *   We should write to the `PC`; but to which byte? 
        (`DHPC` or `PC[L] = data bus, PC[H] = DHPC`)
    *   This means that the *PC Load* control signal is definitely set. The
        instruction being executed is a Branch Absolute, Branch Relative, `JSR`, 
        or `RTS`.
    *   If it's Branch Absolute or Relative, then *loadAR* is `0`; we should
        definitely write to `PC[L]` 
        (semantics: `PC[L] = data bus`, `PC[H] = DHPC`)
    *   If it's `JSR` or `RTS`, then *loadAR* is `1`; we should write to the
        byte specified by the `loadARHorL` control signal.
    1.  If *loadAR* is `0`:
        1.  Return `PC[L] Load Enable` (`PC[L] = data bus, PC[H] = DHPC`).
    2.  Else:
        1.  If *loadARHorL* is `0`:
            1.  Return `PC[L] Load Enable` (`PC[L] = data bus, PC[H] = DHPC`).
        2.  Else:
            1.  Return `DHPC Load Enable`.
2.  Else if *loadAR* is active (`1`):
    *   This means that the current instruction being executed is `LDA`, `MGA`,
        `MAA`, `JSR`, or `RTS`.
    *   If it's one of the following instructions: `MAA`, `JSR`, or `RTS` (i.e.,
        an instruction that writes to both bytes of an AR), then the byte must
        be specified by the *loadARHorL* control signal. (However, note that
        `JSR` and `RTS` only write to the `PC`, so that they are caught by the
        first case (if PC Load Low is active); hence, it must be `MAA`).
    *   If not (it's `LDA` or `MGA`), the byte is specified by the `(AR) L/H`
        instruction operand.
    *   In any of the possible instructions (`LDA`, `MGA,` and `MAA`), the AR is
        specified by the `ARDest` instruction operand.
    1.  If Opcode == `MAA_OPCODE`:
        1.  Return `ARDest[loadARHorL control signal] Load Enable`.
    2.  Else:
        1.  Return `ARDest[(AR) L/H instruction operand] Load Enable`.
3.  Else:
    1.  Return `None`.

**Implementation Diagram:** **TODO**

### 3.  ARSelect

**Inputs:** *SP Out* (control signal), *TmpARWrite* (control signal), 
    Bus Grant signal, FetchCycle signal, Opcode (`INSTR[4:0]`), `ARSrc` 
    instruction operand

**Outputs:** {`None` (Address Bus floats), `PC Output Enable`, 
    `DP Output Enable`, `SP Output Enable`, `BP Output Enable`, 
    `TmpAR Output Enable`}

**Logic:**
1.  If the Bus Grant signal is active:
    1.  Return `None`.
2.  Else if the FetchCycle signal is active:
    1.  Return `PC Output Enable`.
3.  Else if the *SP Out* control signal is active:
    1.  Return `SP Output Enable`.
4.  Else if the *TmpARWrite* control signal is active:
    1.  Return `TmpAR Output Enable`.
5.  Else if the instruction has an `ARSrc` instruction operand:
    *   This means that the instruction has the opcode of any of the following
        instructions: (`MAG`, `LDW`, `SDW`, `MAA`)
    1.  Return `ARSrc Output Enable`.
6.  Else:
    *   Default behavior: write `DP` to the bus.
    1.  Return `DP Output Enable`.

**Implementation Diagram:** **TODO**

### 4.  AddressByteSelect

**Inputs:** *AddressHorL* (control signal), `(AR) L/H` instruction operand,
    Opcode (`INSTR[4:0]`)

**Outputs:** {`Address Bus High Register`, `Address Bus Low Register`}

**Logic:**
1.  If Opcode == `MAA_OPCODE`:
    1.  If *AddressHorL* is active (High):
        1.  Return `Address Bus High Register`.
    2.  Else:
        1.  Return `Address Bus Low Register`.
2.  Else:
    1.  Return `Address Bus {(AR) L/H instruction operand} Register`.

**Implementation Diagram:** **TODO**

### 5.  PC Loading        

Remove the `PC Ld Hi` active low signal, and set `PC[H]` to load whenever 
`PC[L]` loads.

I.e., send the active low `PC Load Low` signal to both `PC[L]` and `PC[H]`, so
that in a cycle where `PC[L]` loads from the data bus, `PC[H]` loads from 
`DHPC`.

##  Updated Instruction Microcode

The microcode of `10` instructions will need to be updated to implement this
proposal. The instructions and their new microcode follows: