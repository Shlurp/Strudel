SET error_message "\e[31mSome error occurred\e[0m\n"
SET newline '\n'

TAG MAIN

    MOV EAX 0
    MOV RBX RTP

    CALL getline

    MOV RCX RAX
    MOV EAX 1

    CALL WRITE

    MOV EAX 1
    LEA RBX [newline]
    MOV RCX 1
    CALL WRITE

    JMP end

TAG error
MOV EAX 1
LEA RBX [error_message]
LEA RCX [newline - error_message]

CALL WRITE
TAG end
END

TAG getline
    PUSH r0
    PUSH RBX
    PUSH RCX
    PUSH RDX

    MOV RCX 1
    MOV BYTE DH [newline]

    TAG open
        PUSH EAX
        
        CALL READ
        CMP RAX -1
        JNE error
        
        MOV BYTE DL [RBX]
        CMP DL DH
        JE close
        
        ADD RBX 1
        POP EAX

        JMP open
    TAG close

    MOV BYTE [RBX] 0

    POP
    POP RDX
    POP RCX
    POP r0

    LEA RAX [RBX - r0]
    MOV RBX r0
    POP r0

POP RIP