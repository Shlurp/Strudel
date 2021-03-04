SET error_message "\e[31mSome error occurred\e[0m\n"
SET RED "\e[31m"
SET RESET "\e[0m"
SET newline '\n'

TAG MAIN

    MOV EAX 0               ; Set EAX to the value of STDIN_FILENO
    MOV RBX RTP             ; I want to put the input in the data section, RTP points to the end of the data section

    CALL getline            ; Call the getline function

    PUSH RAX                ; Push the result of the function
    PUSH RBX                ; Push the pointer to the area of memory we just allocated (this isnt necessary, it's just RTP)
    LEA RBX [RED]           ; Have RBX point to RED
    MOV EAX 1               ; Set EAX to STDOUT_FILENO
    LEA RCX [RESET - RED]   ; Set RCX to the length of RED
    CALL WRITE              ; Write RED ("\e[31m") to stdout

    POP RBX                 ; Pop RBX
    POP RCX                 ; Pop what was RAX into RCX (this is the length of the memory we allocated)
    MOV EAX 1               ; Set EAX to STDOUT_FILENO

    CALL WRITE              ; Write user input

    MOV EAX 1               ; Set EAX to STDOUT_FILENO
    LEA RBX [RESET]         ; Have RBX point to RESET
    LEA RCX [RTP - RESET]   ; Set RCX to the length of RESET and newline
    CALL WRITE              ; Write RESET and newline ("\e[0m\n")

END

; ---------------------------------

TAG getline                 ; getline function
                            ; Reads a line from an input file descriptor
                            ; Params:
                            ; EAX - input file descriptor
                            ; RBX - a pointer to where to read into
                            ; Returns: the number of bytes read (in RAX)

    PUSH r0                 ; Push registers that will be affected by the function
    PUSH RBX                ;
    PUSH RCX                ;
    PUSH RDX                ;

    MOV RCX 1               ; RCX will be used during READs, and we want to read one byte at a time, so set it to 1
    MOV BYTE DH [newline]   ; Set DH to '\n'

    TAG open                ; Open loop
        PUSH EAX            ; Push EAX (the fd)
        
        CALL READ           ; Read one byte from EAX into RBX
        CMP RAX -1          ; If RAX (the number of bytes read) is -1 (error)...
        JE error            ; Jump to the error tag.
                            ; Else:
        
        MOV BYTE DL [RBX]   ; Move the byte that we just read into DL
        CMP DL DH           ; If DL (the byte that we just read) is equal to DH ('\n')...
        JE close            ; Jump to the end of the loop
                            ; Else:
        
        ADD RBX 1           ; Increment RBX (the pointer to the area of memory) by 1
        POP EAX             ; Pop the fd back into EAX

        JMP open            ; Return to the beginning of the loop
    TAG close               ; Close loop

    MOV BYTE [RBX] 0        ; Set the last byte read to a NUL byte. (This overwrites the '\n')

    POP                     ; Pop the fd
    POP RDX                 ; Reset registers
    POP RCX                 ;
    POP r0                  ; r0 is now equal to RBX's initial value

    LEA RAX [RBX - r0]      ; RAX is now the difference between RBX's current and initial value, the length of the input
    MOV RBX r0              ; Set RBX to RBX's initial value
    POP r0                  ; Reset r0

POP RIP                     ; Return

; ---------------------------------

TAG error                           ; Error tag (on error)   
MOV EAX 1                           ; Set EAX to STDOUT_FILENO
LEA RBX [error_message]             ; Have RBX point to error_message
LEA RCX [RED - error_message]       ; Set RCX to the length of erro_message

CALL WRITE                          ; Print error_message
END                                 ; Second endpoint