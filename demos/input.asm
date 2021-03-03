SET red "\e[31m"                ; Red ansi sequence
SET reset "\e[0m"               ; Reset ansi sequence
SET end_msg "That's it folks\n" ; Message

TAG MAIN                    ; Entrypoint

    LEA RBX [RSP]
    ADD RSP 64                  ; Allocate mem for buffer
    MOV EAX 0                   ; Move file descriptor for stdin into eax
    MOV RCX 64                  ; Ready RCX reg for READ func call

    CALL READ                   ; Call READ predefined function

    PUSH RBX                    ; Push RBX onto the stack (to be popped later)
    PUSH EAX                    ; Push EAX (the number of bytes read) onto the stack (to be popped later)

    LEA RBX [red]               ; Move pointer to red string into RBX
    LEA RCX [reset - red]       ; Move length of red string into RCX
    MOV EAX 1                   ; Move 1 (stdout fd) into EAX

    CALL WRITE                  ; Call WRITE predefined function

    POP RCX                     ; Pop top of stack into RCX (this was previously EAX, the number of bytes read)
    POP RBX                     ; Pop top of stack into RBX (this was the bottom of the stack, where the input was read into)
    MOV EAX 1                   ; Move 1 (stdout fd) into EAX 

    CALL WRITE                  ; Call WRITE predefined function

    MOV EAX 1                   ; You get the drill 
    LEA RBX [reset]             ; Move the address of the reset ansi sequence into RBX
    LEA RCX [end_msg - reset]   ; Move the length of the sequence into RCX

    CALL WRITE                  ; Write the sequence

    MOV EAX 1                   ; Move stdout fd into EAX
    LEA RBX [end_msg]           ; Move end message string into RBX
    LEA RCX [RTP - end_msg]     ; Move length of end_msg into RCX

    CALL WRITE                  ; Write the message...

END                         ; Endpoint