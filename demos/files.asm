SET file_name "./demos/files.asm"   ; The path to the file to read from

TAG MAIN                            ; Entrypoint             

    LEA RAX [file_name]                 ; Set RAX to file_name 
    MOV RBX 0                           ; Set RBX to 0

    CALL OPEN                           ; Call predefined OPEN function
                                        ; Parameters:
                                        ; RAX - the name of the file to open
                                        ; RBX - the number of flags to open the file with
                                        ; Returns: (in EAX) the file descriptor of the file opened
                                        ; Notes: in order to open the file with flags (eg. O_RDWR), push each flag onto
                                        ; the stack, and set RBX to the number of flags pushed. These flags will all be or'd together

    PUSH EAX                            ; Push the file descriptor returned onto the stack (for larger programs so it can be accessed again)

    MOV RCX 2300                        ; Move 2300 into RCX
    LEA RBX [RSP]                       ; Move the value of RSP into RBX (MOV RBX RSP would've worked too)

    CALL READ                           ; Call predefined READ function
                                        ; Parameters:
                                        ; EAX - the fd to read from
                                        ; RBX - a pointer to the area of memory to read into
                                        ; RCX - the number of bytes to read
                                        ; Returns: (in RAX) the number of bytes read

                                        ; This function call reads the file onto the stack. (it doesnt change RSP though)

    ADD RSP RAX                         ; Increase RSP by the number of bytes read (so you can see what happens to the stack)

    MOV EAX 1                           ; Move 1 (the fd of stdout) into EAX

    CALL WRITE                          ; Call predefined WRITE function
                                        ; Parameters:
                                        ; EAX - the fd to write to
                                        ; RBX - the pointer to the area of memory to write from
                                        ; RCX - the number of bytes to write
                                        ; Returns: (in RAX) the number of bytes written

END                                 ; Endpoint

; Run: 
; $ ./cstru demos/files.asm 
; $ ./stru -s a.out (scroll up to see the output)