#   ByteFrost Development Log - RTS Microcode Fix

December 25, 2025

##  Overview

The proposed `RTS` microcode implementation from the June 7 - July 12
development log (Addressing Modes Proposal (v2)) has a bug in that it assumes
that telling the `SP` to count (using the *Stack Pointer Count* control signal
in conjunction with the *Stack Pointer Increment / Decrement* control signal)
will cause it to count *on the next rising clock edge*, whereas what really
happens is that the *Stack Pointer Count* control signal **is the clock input
for the `SP[L]`'s (and later will also be `SP[H]`'s, when `SP[H]` is implemented
as a counter) 4-bit counter ICs.**

Therefore, we should treat the *Stack Pointer Count* control signal as telling
the SP to count in the current cycle. This will happen with a short delay after
the rising edge of the clock of the current clock cycle (delay for the control
signals of the current cycle to be produced correctly from the microcode ROMS
and specifically for the *Stack Pointer Count* control signal to reach the
)

**Nevermind, updated SP's semantics to count on next rising clock edge**