SET zero '0'            ; So I can use this for stuff like x - '0'

TAG MAIN

    MOV RBX RSP             ; Allocate memory for input buffer
    MOV RCX 64              ;
    ADD RSP RCX             ; 
    MOV EAX 0               ; Set EAX to stdin fd

    CALL READ               ; READ from stdin
    CALL ATOI               ; Call ATOI
    CALL PRNUM              ; Prints EAX, the integer version of the input

END

; -------------

TAG ATOI                ; ATOI - ascii to integer function
                        ; parameters:
                        ; RBX - input buffer (this isn't RAX because READ returns the buffer into RBX)
                        ; Returns: (In RAX) the value of the integer string RBX

    PUSH RBP                ; Push stack frame pointer onto stack
    PUSH RBX                ; Push regs that'll be changed by the func onto the stack
    PUSH RCX                ; 
    PUSH r0                 ;

    MOV RAX 0               ; Set RAX to 0
    MOV BYTE r0 [zero]      ; Set r0 to '0' (48)

    TAG open                ; Opening of loop
        MOV BYTE CL [RBX]       ; Move the byte at RBX into CL (RBX is the iterator)
        CMP CL 0                ; Compare CL to NUL
        JE close                ; Jump to end of loop if CL is NUL
        CMP CL 10               ; Compare CL to '\n'
        JE close                ; Jump to end of loop if CL 

        PUSH RBX                ; Push RBX (we need to change it for MUL)
        MOV RBX 10              ; Set RBX to 10
        CALL mul                ; Call mul function (RAX *= 10)

        LEA RAX [RAX + RCX - r0]; RAX += RCX - '0'
        POP RBX                 ; Set RBX to original value

        ADD RBX 1               ; Increment RBX by 1

        JMP open                ; Jump to beginning of loop
    TAG close               ; Close loop tag

    POP r0                  ; Restore reg values
    POP RCX                 ;
    POP RBX                 ;
    POP RBP                 ; Restore frame pointer
POP RIP                 ; Restore instruction pointer

; ---------

TAG mul                 ; mul function tag (this isn't useful now, this was written before the MUL instruction)
                        ; Parameters:
                        ; RAX: number 1 
                        ; RBX: number 2
                        ; Returns: (In RAX) RAX * RBX

    PUSH RBP                ; Push stack frame pointer
    PUSH r0                 ; Push regs that will be affected by the function
    PUSH r1                 ;

    MOV r1 0                ; Set regs to 0
    MOV r0 0                ;

    TAG open1               ; Open loop tag
        CMP r0 RBX              ; Compare r0 (iterator) with RBX
        JGE close1              ; Jump to end of loop if r0 >= RBX

        ADD r1 RAX              ; Add RAX to r1

        ADD r0 1                ; Increment r0 by 1

        JMP open1               ; Jump to beginning of loop
    TAG close1              ; Closing tag

    MOV RAX r1              ; Move r1 (the result of the product) into RAX

    POP r1                  ; Restore register values
    POP r0                  ;
    POP RBP                 ; Restore stack frame pointer
POP RIP                 ; Restore instruction pointer