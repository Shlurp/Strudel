TAG MAIN            ; Entrypoint

TAG open            ; Opens loop 
PUSH 100            ; Pushes 100 onto the stack
ADD RAX 1           ; Increments iterator
CMP RAX 10          ; Compares iterator with 10
JLE open            ; Jumps to opening of loop if iterator is less than or equal to 10
PUSH 0              ; Pushes 0 onto the stack

TAG open1           ; Opens loop (overwrites our previous "open" tag)
PUSH 50             ; Pushes 50 onto the stack
SUB RAX 1           ; Decrements iterator
CMP RAX 5           ; Compares iterator with 5
JG open1            ; Jumps to opening of loop if iterator is greater than 5

PUSH 0              ; Pushes 0 onto the stack
END                 ; End point

; This code is the same as:
;
; do{
;   PUSH 100; // This is obviously not an actual function
;   i ++;
; }while(i <= 10);
;
; PUSH 0;
;
; do{
;   PUSH 50;
;   i --;
; }while(i > 5);
;
; PUSH 0
;
; Run: $dum -srf demos/do_while.asm