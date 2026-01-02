# Look Up Table EEPROMs

**Last Updated:** June 15, 2025

Some of the ByteFrost's hardware units (e.g., **AR Data Bus Load Enable** and
**ARSelect**) require complex combinational logic to be properly implemented.
This complexity translates to a relatively large number of logic gates and other
components, which in turn translates into a large number of ICs, requiring a
large amount of physical area to implement. In addition, the higher a hardware
implementation's complexity, the higher the chance for errors. A hardcoded
circuit implementation with logic gates can be difficult to debug and fix,
especially in a breadboard environment. Even if everything is correct,
maintenance will also be more difficult since more ICs = more components that
can potentially result in failures.

Hence, in order to minimize hardware implementation area cost, lower complexity,
and increase testability and maintainability, we can implement complex
combinational circuits using LUTs (look-up tables), implemented using EEPROMs.
The idea is that the EEPROMs act as a black box encompassing the implementation,
with each 1-bit input of the combinational logic circuit being assigned and sent
to one of the EEPROM's address bits, and each 1-bit of output of the circuit is
assigned and outputted from one of the EEPROM's data output bits.

In order to set an EEPROM's LUT contents, we need to generate a binary file that
contains the LUT. In this folder is the software for doing so and binary files
for LUTs used by the ByteFrost.



**NOTE: Issues with programmer configuration - choose CAT28C16A**

