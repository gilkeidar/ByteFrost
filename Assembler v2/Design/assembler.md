#   ByteFrost Assembler

An `.asm` file is a file in which each line may contain at most one assembly
statement.

A **token** is a non-empty string that contains no whitespace characters
    (i.e., its length is $\geq 1$).

A line is converted into a list of tokens (e.g., `vector<string>`).

A line may also contain a comment. This means that some token contains the
comment start substring `\\`. All tokens in a token list after and including
this token can be ignored (don't add them to the line's token list).

Each line's token list is then converted to a corresponding `Line` object by the
assembler's parser in the following way:

Given a line string `S` and its list of tokens `T`:

1. If there are no tokens in `S` (i.e., `T.size() == 0`), then `S` contains only
    whitespace. Its corresponding line object `L` is set to `Empty`.
2. If there is at least one token in `S` (i.e., `T.size() >= 1`):
    1. If the first character of the first token in `S` is `:` or `.`, then:
        1. If the first character of the first token in `S` is `:`, then this
            line is a label definition. This line can only contain one token,
            the label definition itself.
        2. If the first character of the first token in `S` is `.`, then this
            line is a preprocessor directive. The kind of preprocessor directive
            is specified in the rest of the token starting with the `.`
            (i.e., the substring starting at index 1 and extending to the end
            of the first token in `T`). If this substring is unknown, the line 
            is an invalid preprocessor directive. If it is known, the number of
            expected tokens and their types (i.e., string, immediate, size, etc.)
            is also known and must be matched in the line (if not, the line is
            an invalid preprocessor directive).
    2. If the first token is the name of an assembler-recognized instruction,
        then:
        1. The number and types of subsequent tokens must match one of the 
            expected token type lists of this instruction; otherwise, this is
            an invalid instruction line.
    3.  Otherwise, this line is invalid and the parser should mark it as such.

##  Parsing

The parser is the first entity that runs in the ByteFrost assembler when it is
given a `.asm` file.

The parser iterates over each line string of the file `s`, which ends with
either `\n` or `CRLF`.
The parser then does the following:

Given the line string `s`:
1. Let `s'` = substring of `s` from index 0 to the index of the character
    immediately before the first character of the first instance of `\\`. If
    no instance of `\\` exists in `s`, then `s' = s`.
2. Convert `s'` to a list of non-empty strings `vector<string>` by considering
    whitespace and commas as delimeters. (May more generally define a delimiter
    set and if any character in `s'` is within the set, finish the current
    token string and begin the next token string once a character not in the
    delimiter set is encountered).
3. Iterate over each string `w` in the `vector<string>` generated from `s'`.
    Note that every string `w` does not contain any character in the delimiter
    set.
4. For every string `w` in the `vector<string>` generated from `s'`:
    1. Create a `Token` `t` that `w` may be mapped to. These are the
        assembler-recognized token types:
        0. First, some definitions: 
            1.  $A$ is the set of upper and lower case
            English characters.
            2.  $D$ is the set of 10 numerical digits.
        1. `TEXT`. `w` is mapped to a `Token` of type `TEXT` iff
            $w \in (A \cup \{\_ \})~\circ~(A \cup \{\_\} \cup D)^*$
            0. $TEXT = (A \cup \{\_ \})~\circ~(A \cup \{\_\} \cup D)^*$
            1. In other words, the first character of `w` is an underscore or
            in $A$, and any following characters may be an underscore, in $A$,
            or in $D$.
        2. `NUMBER`. `w` is a string that denotes a decimal or hexademical
            number.
            1. An unsigned decimal number string is in the set
                $ND = \{0\} \cup (D \ \{0\})D^*$
            2. An unsigned hexadecimal number string is in the set
                $NH = \{0x\} \circ D^+$
            3. An unsigned number string is therefore in the set
                $N = ND \cup NH$
            4. A number string is therefore in the set
                $\{\epsilon, -, +\} \circ N$
            5. Hence, `w` is mapped to a `Token` of type `NUMBER` iff
                $w \in \{\epsilon, -, +\} \circ N$
        3. `IMMEDIATE`. `w` is a string that denotes an immediate iff
            $w \in \{\#, -, +\} \circ N$
        4. `LABEL`. `w` is a string that denotes a label iff
            $w \in \{:\} \circ TEXT$.
        5. `DIRECTIVE`. `w` is a string that denotes a preprocessor directive
            iff $w \in \{.\} \circ TEXT$.
        6.  `INVALID`. `w` denotes an invalid token if `w` cannot be mapped to
            one of the above `Token` types.

##  Line Types

Here is a more in-depth discussion of the possible line types. All line types
contain no comments (comments are ignored by the parser and are not included in
any line's token list).

A line's type is ascertained by its token list, `T` (`vector<Token>`).

### Whitespace Line

A whitespace line is a line that contains no tokens; i.e., `T` is empty.

### Preprocessor Directive Line

A preprocessor directive line is a line such that:

1. `T.size() >= 1`
2. The first character of the first token in `T` is `.`
3. The substring `T[0][1:end]` is the name of a preprocessor directive
    recognized by the assembler; the preprocessor directive is `D`.
4. The number of tokens in the line is `1 + x` where `x` is the number of tokens
    expected by at least one of the assembler-recognized token patterns for `D`.

### Label Definition Line
