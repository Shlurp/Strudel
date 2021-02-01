TAG MAIN        ; Entrypoint
MOV RAX 5       ; FACTORIAL takes its input from RAX
CALL FACTORIAL  ; Call FACTORIAL function
PUSH RAX        ; Push the value returned by FACTORIAL onto the stack
END             ; Endpoint


TAG MUL         ; MUL (multiply) function (input: RAX and RBX, output: RAX * RBX)
PUSH RBP        ; Push the stack frame pointer onto the stack
MOV RBP RSP     ; Update stack pointer
PUSH r1         ; Push values of r1 and r2 onto the stack so they can be returned to their original state
PUSH r2         ;
MOV r1 0        ; Move 0 into r1 and r2
MOV r2 0        ; 

TAG open1       ; TAG for loop (as TAGs are global- there is no concept of scopes in Dumdum, 
                ; in order for this tag to not be overwirtten, I gave it a number)
CMP r1 RBX      ; Compare r1 and RBX
JGE close1      ; If r1 (the iterator) is larger or equal to the RBX, jump to close1 (break)

ADD r2 RAX      ; Add RAX to r2
ADD r1 1        ; Increment the iterator by 1
JMP open1       ; Jump to the beginning of the loop

TAG close1      ; End of loop tag

MOV RAX r2      ; Move r2 into RAX
POP r2          ; Pop r2's old value into r2
POP r1          ; Pop r1's old value into r1
POP RBP         ; Pop the old frame pointer into RBP
POP RIP         ; Return to the caller


TAG FACTORIAL   ; FACTORIAL function (input: RAX, output: RAX!)
PUSH RBP        ; Push the stack frame pointer onto the stack
MOV RBP RSP     ; Update stack pointer
MOV RBX RAX     ; RAX will hold the result, so move the input into another register, RBX
MOV RAX 1       ; Move 1 into RAX and r1
MOV r1 1        ;

TAG open2       ; Open loop tag
CMP r1 RBX      ; Compare r1 with RBX
JG close2       ; If r1 is greater than RBX, exit loop
PUSH RBX        ; Push RBX onto the stack (so we can get its value later)
MOV RBX r1      ; Move r1 into RBX
CALL MUL        ; Call MUL (this returns into RAX, and RBX has the iterator value)
POP RBX         ; Restore RBX's original valie
ADD r1 1        ; Increment the iterator
JMP open2       ; Jump to the beginning of the loop
TAG close2      ; End of loop tag 

POP RBP         ; Pop the old frame pointer into RBP
POP RIP         ; Return to the caller

; In C:
;
; int mul(int x, int y){
;   int product = 0;
;   int i = 0;
;   
;   for(i=0; i<y; i++){
;       product += x;
;   }
;   
;   return product;
; }
;
; int factorial(int x){
;   int fac = 1;
;   int i = 1;
;
;   for(i=1; i<=x; i++){
;       fac = mul(fac, i);
;   }
;
;   return fac;
; }
;
; void main(){
;   int fac = factorial(5);
; }
;
; Run: $dum -s demos/factorial.asm
; It will take longer if you use any other flags, so I recommend reducing the value of RAX (on the second line)
