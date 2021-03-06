TAG MAIN

    MOV EAX 10
    CALL fib
    PUSH RAX

END


TAG fib
    PUSH RBP
    MOV RBP RSP
    PUSH 0

    TAG calculate
    LEA RBX [RAX - 1]
    MOV r0 0
    MOV r1 1
    MOV r2 1
    MOV r3 0

    TAG open
        CMP r0 RBX
        JGE close

        LEA r3 [r1 + r2]
        MOV r1 r2
        MOV r2 r3

        ADD r0 1

        JMP open
    TAG close

    MOV QWORD [RBP] r2

    TAG cleanup
    POP RAX
    POP RBP
POP RIP
