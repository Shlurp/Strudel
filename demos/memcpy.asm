SET test "\e[31mHello\e[0m!\n"

TAG MAIN
    LEA RBX [test]
    MOV RAX RTP
    MOV RCX 17

    CALL memcpy

    LEA RBX [test]
    MOV RCX 34
    MOV EAX 1

    CALL WRITE
END

TAG memcpy

    PUSH RAX
    PUSH RBX
    PUSH RCX

    TAG open
        CMP RCX 0
        JLE close

        MOV BYTE [RAX] [RBX]

        ADD RAX 1
        ADD RBX 1
        SUB RCX 1

        JMP open

    TAG close

    POP RCX
    POP RBX
    POP RAX

POP RIP
