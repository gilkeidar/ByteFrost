# ByteFrost 
![ByteFrost Logo](Logo.png)
The Breadboard 8-bit CPU and Computer

Be sure to check out the [YouTube channel](https://www.youtube.com/@bytefrost) for more in-depth explanations of how the ByteFrost works!

In this Github repository are software tools for using the ByteFrost computer:

## Assembler
Assembler program that converts Assembly code to ByteFrost machine language.

## Display
Arduino code for the ByteFrost's display card (an Arduino Nano) for a 2004A LCD display.

## Documentation
Documentation for the ByteFrost computer. Includes hardware documentation (i.e. schematics) and software documentation (instruction set specifications, control signal lists, each instruction's micro-code and structure, and assembly syntax). For more documentation, see the [wiki](https://github.com/gilkeidar/ByteFrost/wiki).

## Microcode
Arduino code for programming the three microcode EEPROMs (using Ben Eater's [EEPROM programmer](https://github.com/beneater/eeprom-programmer/blob/master/eeprom-programmer/eeprom-programmer.ino) as a basis).

## Programs
Programs written in ByteFrost assembly that can be compiled using the assembler and run on the ByteFrost computer. Each program comes with the assembly code (`.asm` file), machine language compilation (`.mlg` file), and a binary file (`.bin` file).
