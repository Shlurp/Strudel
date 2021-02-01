PUSH 700        ; This won't happen - it's before the entrypoint
TAG MAIN        ; Sets the entrypoint
MOV RBP RSP     ; Not necessary for the entrypoint - I'll explain later
PUSH 100        ; Pushes 100 onto the stack
CALL fun        ; Calls the "subroutine" fun
PUSH 999        ; Pushes 999 onto the stack
MOV QWORD [RSP] 0 ; The value located at the stack pointer is made 0
END             ; End the program

TAG fun         ; Declares a tag, in this case, it's used as a subroutine
PUSH RBP        ; Pushes the frame pointer onto the stack
MOV RBP RSP     ; Moves the stack pointer into the frame pointer - allowing for local variables
PUSH 200        ; Pushes 200 onto the stack
LEA RSP [RSP - 8] ; Moves RSP back 8
POP RBP     ; Gets the frame pointer
POP RIP     ; The instruction pointer was pushed onto the stack by CALL, and now this returns to the calling func
;
; This is the same as:
;
; void fun(){
;   int x = 200;    
; }
; 
; void main(){
;   int x = 100;
;   fun();
;   int y = 999; 
; }
;
; Run: $dum -sir demos/subroutine.asm
