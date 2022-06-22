# ByteFrost Assembler
The ByteFrost assembler may be used to compile `.asm` files containing ByteFrost assembly code to either `.mlg` files (ByteFrost machine language written in ASCII) or `.bin` files (complied ByteFrost assembly in binary format)

Usage: `./assembler program_name [-b]`

## Flags

Flag | Usage
--- | --- 
`b` | Specifies to compile assembly into a binary file (`.bin`).<br>If not specified, the assembler will compile to `.mlg` by default.
