# STRUCTURE

+ Header:
    + Magic - Sequence of bytes denoting that this is a compiled Strudel file.
    + Version (12 bytes)- Version number
    + Entrypoint (8 bytes)- The offset of the entrypoint in the code.

+ Data:
    + Text data section length (8 bytes) - The length of the next segment
    + Data section - This is the data section, as initialized by `SET`.

+ Bytecode:
    + Bytecode - the bytecode of the program