#   Definitions

##  General Definitions

**`.asm` file** - a ByteFrost assembly file, each line of which may contain at
most one assembly statement.

**Token** - A *token* is a non-empty string that contains no delimiter 
characters and which is of an identified token type (meaning that the relevant
parser has marked the token's string with a type identifier that relates to its
semantics and expected use). The delimiter characters and possible token types
differ by parsers and context (e.g, the CLAP (*Stage 0*) uses different 
delimiters and token types than the Parser (*Stage 1*); hence the relevant
structs and enums are defined differently).

##  Parsing Set Definitions

These set definitions are used in the parsing stages of the assembler pipelines.
Every token string is mapped to a token type (either `CLTokenType` in the CLAP 
or `TokenType` in the Parser) at least in part by checking whether the token
string or substrings of it are members of some of these sets.

### Character (Symbol) Sets

1.  $ASCII$ is the set of ASCII characters.
    * $ASCII = \{x~|~ x \text{ is an ASCII character}\}$
2.  $A$ is the set of upper and lower case English characters.
    * $A = \{x~|~ x \text{ is an upper or lower case English character}\}$
3.  $D$ is the set of 10 numerical digits (0 - 9).
    * $D = \{0, 1, 2, 3, 4, 5, 6, 7, 8, 9\}$
4.  $H$ is the set of hexadecimal digits.
    * $H = D \cup \{a, b, c, d, e, f, A, B, C, D, E, F\}$

### String Sets

**Note:** In this context, $\epsilon$ is used to mean the empty string.

5.  $TEXT$ is the set of possible variable names.
    * A string $s$ is in $TEXT$ iff the first character of $s$ is an underscore
     or in $A$, and every character in $s$ after the first character may be an
     underscore, in $A$, or in $D$.
    * $TEXT = (A \cup \{\_\})~\circ~(A \cup \{\_\} \cup D)^*$
6.  $ND$ is the set of unsigned decimal number strings.
    * $ND = \{0\} \cup (D \setminus \{0\})D^*$
7.  $NH$ is the set of unsigned hexademical number strings.
    * $NH = \{0x\} \circ H^+$
8.  $N$ is the set of unsigned number strings.
    * $N = ND \cup NH$
9.  $NUMBER$ is the set of (hexademical and decimal) number strings.
    * $NUMBER = \{\epsilon, -, +\} \circ N$
10. $FILE$ is the set of file names.
    * $FILE = TEXT \circ \{.\} \circ TEXT$
    * **Note:** This definition is possibly more restrictive than most file
    systems regarding acceptable file names. It may be prudent to redefine this
    set to allow for more expressive file names.
11. $INSTRUCTIONS$ is the set of recognized assembly instruction names.
    * $INSTRUCTIONS \subseteq TEXT$.
    * String membership in $INSTRUCTIONS$ can be checked by using the 
    `Assembler`'s `std::unordered_map<std::string, std::vector<AssemblyInstruction>> instructions` hashmap, initialized in the
    `Assembler` constructor.
12. $GREGISTERS$ is the set of general purpose register names.
    * $GREGISTERS \subseteq TEXT$.
    * $GREGISTERS = \{\texttt{R0}, \texttt{R1}, \texttt{R2}, \texttt{R3}\}$
13. $SREGISTERS$ is the set of special register names.
    * $SREGISTERS \subseteq TEXT$.
    * $SREGISTERS = \{\texttt{DHPC}, \texttt{HDP}, \texttt{HSP}\}$
    * **Note:** $SREGISTERS$ strings do NOT contain the `%` special register
    prefix; they are only the special register names.
14. $BYTE\_SELECT$ is the set of possible instances of byte selection (array
    indexing syntax)
    * $BYTE\_SELECT = \{\texttt{[}~\}~\circ ND~\circ \{~\texttt{]}\}$ 