TAG MAIN

    MOV RBX 6       ; Move 5 into RBX, this will be the index of the fibonacci number to calculate (0:1, 1:1, 2:2, 3:3, 4:5...)

    CALL Fib        ; Call the Fib function
    CALL PRNUM      ; Call the predefined PRNUM function (prints RAX)

END

TAG Fib             ; Fib function:
                    ; Parameters:
                    ; RBX - the index of the number to calculate
                    ; Returns:
                    ; The fibonacci number at index RBX (in RAX)
    
    PUSH RCX        ; Push RCX onto the stack. This will be altered by the function, and we want to preserve its initial state,
                    ; both for recursive purposes and etiquette.
    PUSH RBX        ; Same goes for RBX, but this is for recursive purposes only

    CMP RBX 1       ; Compare the index to 1...
    JG recurs       ; If the index is greater than 1, jump to the recursive part of the function
    MOV RAX 1       ; Else set RAX to 1...
    JMP end         ; And jump to the end

    TAG recurs      ; Recursive part of the function Fib
    SUB RBX 1       ; Subtract 1 from RBX
    CALL Fib        ; Call Fib recursively (Fib(RBX - 1))
    MOV RCX RAX     ; Move the result of the function into RCX
    SUB RBX 1       ; Subtract one more from RBX
    CALL Fib        ; Call Fib recursively again (Fib(RBX -2))
    ADD RAX RCX     ; Add the previous result (Fib(RBX - 1)) to the result (Fib(RBX - 2))
                    ; This will be returned, so you're essentially returning Fib(RBX - 2) + Fib(RBX - 1)

    TAG end         ; End tag (like cleanup)
    POP RBX         ; Pop the reigisters that were pushed for preservation
    POP RCX         ;
    
POP RIP             ; Return