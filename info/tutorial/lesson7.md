# **LESSON 7**
*CALL*

The `CALL` function is essentially a specialized `JMP` instruction. It has one usage:  
`CALL` \<Tag>  
`CALL` pushes the current offset of the program, stored in the `RIP` register, onto the stack and then jumps to the tag. The reason why this must be an atomic action will become clear later.  
Usually the code for a `CALL` instruction will lookm something like:

```
CALL fun

; fun function
TAG fun

PUSH RBP
MOV RBP RSP

; Do stuff

MOV RSP RBP
POP
POP RBP
POP RIP
```

So this is what happens:
1. `RIP`, the instruction pointer, is pushed onto the stack
2. The program jumps to **fun**
3. `RBP`, the stack frame pointer, is pushed onto the stack
4. `RBP` is updated with the value of `RSP`, the stack pointer
5. Execute code
6. Move `RSP` back to its previous value before the function's code.
7. Pop the old value of `RBP` into `RBP`
8. Pop the old value of `RIP` into `RIP` so the program returns to right after `CALL fun`.

The reason why `CALL` must be atomic may be apparent now. If instead you did:

```
PUSH RIP
JMP fun
```

Then `RIP` would have the value of `JMP fun`, and would probably lead to an infinite loop.  
If you did instead:

```
JMP fun
PUSH RIP
```

Then you'd only push `RIP` after returning (somehow) from fun. But as you didn't push `RIP` before jumping, there's no way (except for using another tag) to return from **fun**.  

The reason we use `RBP` is so we can create a form of scoping. Notice how after the `MOV RBP RSP` instruction, all `PUSH`es can be accessed through offsets from `RBP`. So say you `PUSH` some value onto the stack, it can be referenced by `RBP`. `PUSH` another value and its `RBP + 8`, and so on.  

***

There are a few tags that should not be overwritten (do not use `TAG` on them), as they are predefined functions (or entrypoint):

+ `MAIN` - this is the entrypoint of your program. Execution starts here.
+ `OPEN` - this is a predefined function. We will discuss it later.
+ `READ` - this is a predefined function. We will discuss it later.
+ `WRITE` - this is a predefined function. We will discuss it later.

Do not name tags with instruction names (ie. tags like `TAG MUL` will result in an error, as `MUL` is already an instruction)

The instruction `END` exits the program, it is necessary so if you have code like:
```
TAG MAIN

; code
CALL fun
; code

END

TAG fun
; code
POP RIP
```

The program doesn't execute **fun** after `MAIN` is finished executing. Because if we didn't write `END`, we'd venture into the realm of undefined behavior.

*** 

For a demo of the `TAG`, `JMP`, conditional jumps, and `CALL`, look [here](../../demos/tutorials/funcs.asm)

**Challenge:** write a function to calculate some element in the fibonacci sequence. The function **fib** will take `EAX` as input, and will calculate the `EAX`th element in the fibonacci sequence.
<details>
<summary><b>Solution</b></summary>

A solution can be found [here](../../demos/fibonacci.asm)
</details>