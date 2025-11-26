#   ByteFrost Development Log - Assembler Updates

July 20, 2025

##  Overview

The ByteFrost Assembler must be updated to support the new ISA instructions and
new syntax for some of the Assembly instructions. In this development log, we
shall provide a complete list of ISA instructions and Assembly instructions that
the Assembler will need to support, as well as the new token types required for
some of the new Assembly instructions.

### New `TokenType`s

In this proposal, we'll add the following `TokenTypes`:
1.  `AR_L`  (for `ARSrc` low / high distinctions, e.g. `LDWL` vs. `LDWH`)
2.  `AR_H`
3.  `AR`    (for `ARSrc` when no distinction needs to be made or `ARDest`)

We'll also deprecate the following `TokenTypes`:
1.  `SREGISTER`

