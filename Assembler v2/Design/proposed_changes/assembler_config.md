#   Assembler `Config` Object Proposal

May 13, 2025

**Proposal:** Add a `Config` class that contains config metadata information
regarding the `Assembler`'s behavior; the `Assembler` will contain an instance
of a `Config` as a field (initialized with default values or from an external
config file), a reference to which will be passed as an argument to every
stage class instance.

This can then be used to implement message passing between stages; for instance:

Consider the problem of specifying a start address to the assembler. Currently,
the assembler assumes that the first instruction in an assembly file will be
stored in memory at address `0x0000`, so all label addresses are calculated
based on this assumption. This only works when the assembly file is of a program
that is intended to be stored and run in the ROM; if a program should be loaded
into RAM at, say, address `0x3000`, then using this approach, all branches in
the program will break when loading to address `0x3000`.

Hence, the assembler should be notified that it should offset all label
addresses forward by `0x3000` (or in other words, that the start address is
`0x3000`). This can be done using a command-line argument and / or introducing
a new preprocessor directive (e.g., `.start 0x3000`).

We may now begin to see the problem: the pipeline stage in the assembler that
learns of the change in default behavior (e.g., the CLAP or Preprocessor in this
example) is not the same stage that *uses* the new behavior (in this case, the
Label Handler).

Hence, we need a way for the CLAP / Preprocessor to communicate to the Label
Handler that it should use a different starting address.

We can do this arbitrarily by adding a global `uint16_t start_address` field to 
the `Assembler` and pass a reference to it to the `CLAP`, `Preprocessor`, and
`LabelHandler` constructors / `run()` methods; however, this would mean that if
we need to do something like this again for some other purpose, we would need to
break the interface between the pipeline stages and the Assembler again (by
changing their constructor / `run()` method signatures).

Instead, we can use a more general solution: add a `Config` class to the
`Assembler` that contains all such metadata that affects the behavior of any of
the pipeline stages; its fields can be set to defaults in its constructor or its
defaults could be stored in an external config file that the Assembler reads
upon construction, or something to this effect. Then, we can pass a reference of
this `Config` instance to every pipeline stage in its constructor / `run()`
method signatures (I think constructor might be nicer, since otherwise private
methods in pipeline stage classes would need to have an additional `Config &` 
argument which could be annoying / hurt code readability).

Since they're all references to one shared instance, the Preprocessor could mark
`config.start_address` to `0x3000` upon handling the `.start` directive, and the
Label Handler would read `config.start_address` when performing label address
calculations (and read the new `0x3000` value).

##  Implementation

The implementation of the `AssemblerConfig` class would involve the following:

In `shared_types.hpp`: (define `AssemblerConfig` struct)

```c++
//  ...
struct AssemblerConfig {
    //  The memory address (not PC address) of the first instruction; i.e., the
    //  address at which this program will be loaded into memory (in the case
    //  of programs in RAM), or the address at which this program is stored (in
    //  the case of a program in ROM).
    uint16_t start_address = 0x0;
}
```

In `assembler.hpp`: (Add `AssemblerConfig` instance to the `Assembler` class)

```c++
class Assembler {
public:
    //  ...
private:
    //  Global data

    AssemblerConfig config;

    //  ...
}
```

In `assembler.cpp`: (Pass reference to `Assembler`'s `AssemblerConfig` struct to
    every pipeline stage)

```c++
Assembler::Assembler(int argc, char ** argv) : clap(argc, argv, config),
    parser(config), preprocessor(config), label_handler(config),
    file_generator(config) {
    //  ...
}
```

In `clap.hpp`: (Add `AssemblerConfig &` struct field and argument)

See: https://stackoverflow.com/questions/12387239/is-it-idiomatic-to-store-references-members-in-a-class-and-are-there-pitfalls

