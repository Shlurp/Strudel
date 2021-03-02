# **LESSON 3**
*MOVing*

So far we'ev discussed `PUSH` and `POP`, but there's another, debatably more powerful instruction: `MOV`. The `MOV` instruction is one of the most versatile instructions. Essentially what it does is move a value from one location to another.  
It has quite a few different uses:

+ `MOV` **\<REG> \<NUM>** - Sets the value of **REG** to **NUM**. (eg. `MOV` **RAX** **100** sets `RAX` to 100).
+ `MOV` **\<REG 1> \<REG 2>** - Sets the value of **REG 1** to the value of **REG 2** (eg. If `RBX` is 3141: `MOV` **RAX** **RBX** sets `RAX` to 3141).
+ `MOV` **\<SIZE> \<REG> \<\[POINTER\]>** - Moves **SIZE** bytes located at **POINTER** to **REG** (eg. if `r1` is pointing to some string whose first character is 'H', `MOV` **BYTE** **RAX** **\[r1\]** sets `RAX` to 72 (the ascii value of 'H')). I'll discuss the different sizes later on.

+ `MOV` **\<SIZE> \<\[POINTER\]> \<NUM>** - Moves **NUM** to the area pointed to by **POINTER** with a size of **SIZE** (eg. `MOV` **BYTE** **\[r1\]** **72** moves one byte whose value is 72 into the memory pointed at by `r1`).
+ `MOV` **\<SIZE> \<\[POINTER\]> \<REG>** - Moves **SIZE** bytes of memory from **REG** to the memory pointed to be **POINTER**. (eg. if `RAX` is 2718, `MOV` **QWORD** **\[RSP\]** **RAX** sets the value at the top of the stack to 2718).
+ `MOV` **\<SIZE> \<\[POINTER 1\]> \<\[POINTER 2\]>** - Moves **SIZE** bytes from the memory pointer to by **POINTER 2** into the the memory pointed to by **POINTER 1** (eg. if `RAX` points to the bottom of the stack, `MOV` **QWORD** **\[RSP\]** **\[RAX\]** sets the top of the stack to the value at the bottom of the stack).

So, a general `MOV` instruction looks like:
> `MOV` \[SIZE\] \<Value to move into> \<Value to move from>

As I briefly discussed, some `MOV` instructions require a **SIZE** parameter. This is so the interpreter knows how to dereference the pointer.  There are four **SIZE**s:
+ **BYTE** - 1 byte
+ **WORD** - 2 bytes
+ **DWORD** - (double word) 4 bytes
+ **QWORD** - (quad word) 8 bytes  

Another important note is that pointer arithmetic is supported in Strudel. A `MOV` instruction like:  
> `MOV` QWORD RAX [RSP - 8]  

Is valid. 

***
For a demo on the `MOV` instruction, look [here](../../demos/tutorials/mov.asm)  
To run:  
```
$ ./cstru demos/tutorials/mov.asm
$ ./stru -sr a.out
```

**Challenge:**
Using only the `MOV`, `PUSH`, and `POP` instructions, and the `RAX` and `RBX` registers, write a program to switch their values. (Assume they are already set and have values)

<details>
<summary>Solutions</summary>
Here are two solutions:

```
TAG MAIN

PUSH RAX
PUSH RBX
POP RAX
POP RBX

END
```

```
TAG MAIN

PUSH RAX
MOV RAX RBX
POP RBX

END
```
</details>