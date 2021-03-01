TAG MAIN

CALL fun            ; Call fun
PUSH RAX            ; Push the return value onto the stack
MOV QWORD [RSP] 0   ; For clarity, move 0 onto the bottom of the stacks

END

; fun function
TAG fun

PUSH RBP            ; Push frame pointer
MOV RBP RSP         ; Update frame pointer

PUSH 0              ; Push 0 onto the stack. This will be our local var

TAG open            ; Open loop tag
MOV QWORD RAX [RBP] ; Move local var into RAX
CMP RAX 10          ; compare RAX with 10
JGE close           ; if RAX >= 10, jump to end of loop

ADD RAX 1           ; RAX ++
MOV QWORD [RBP] RAX ; Move RAX into local var

JMP open            ; Jump to beginning of loop
TAG close           ; End loop tag

POP RAX             ; Pop local var into RAX
POP RBP             ; Pop stack frame pointer into RBP
POP RIP             ; Pop instruction pointer