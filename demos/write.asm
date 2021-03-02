TAG MAIN                        ; Entrypoint

SET hellomsg "Hello World!\n"   ; Creates a global variable, "hellomsg", whose value is now "Hello World\n". 
                                ; "hellomsg" now equals the address that the value is stored at
SET len 0                       ; Creates a global variable, "len", whose value is now 0. "len" points to this value
                                ; "len" is a pointer to a QWORD

LEA RCX [len - hellomsg - 1]    ; RCX now equals the difference between the addresses of "len" and "hellomsg" and 1.
                                ; They are stored right next to each other, so the difference of their addresses is the
                                ; length of "hellomsg". "hellomsg" is NUL terminated, so subtracting 1 removes the NUL byte 
                                ; from the length.

MOV QWORD [len] RCX             ; Move the length of "hellomsg" to "len"

MOV RCX 0                       ; Set RCX to 0 (this is unnecessary, I just did it to show that you can)

; Set up registers for function call
MOV EAX 1                       ; Set RAX to 1 (the file descriptor of STDIN)
LEA RBX [hellomsg]              ; Move the address of hellomsg into RBX
MOV QWORD RCX [len]             ; Move the length of hellomsg into RCX

CALL WRITE                      ; Call WRITE. WRITE is a predefined function, and it takes three parameters:
                                ; EAX - the file descriptor of the file to write to
                                ; RBX - a pointer to the buffer to write 
                                ; RCX - the number of bytes to write to the file
                                ; Returns: The number of bytes written (into EAX)

PUSH EAX                        ; Push the value of EAX (the number of bytes written) onto the stack

MOV EAX 1                       ; Since EAX was changed by the function call WRITE, set EAX to 1 again
MOV RBX RSP                     ; Set the pointer to write to RSP
PUSH 8022916924116329800        ; Push 8022916924116329800 onto the stack. This is the little endian numerical value of "Hello Wo"
PUSH 43509902450                ; Push 43509902450 onto the stack. This is the little endian numerical value of "rld!\n"
CALL WRITE                      ; Call the WRITE function

SUB RSP 16                      ; Move RSP back 16
MOV QWORD [RSP] 0               ; Set value at the top of the stack to 0

END                             ; Endpoint

; In the near future, support for hex values will be implemented, so pushing strings onto the stack will be easier.
; Run: 
; $ ./cstru demos/write.asm
; $ ./stru a.out