# **LESSON 6**
*Conditional jumps and CMP*

We have discussed `JMP`s, which are unconditional jumps, but there are also conditional jumps, which only jump to the tag if a certain criteria is met.  
Before we delve into those, there's another instruction we must discuss: `CMP`. It has the following use:  
`CMP` \<REG 1 / NUM 1> \<REG 2 / NUM 2>  
Simply, this can be written as:  
> `CMP` \<PARAM 1> \<PARAM 2>  

`CMP` subtracts **PARAM 2** from **PARAM 1**, **without changing either of the parameters, and sets the program's flags accordingly. Strudel has 2 flags:

+ `ZF` - zero flag. This is set if the result of `CMP` is 0
+ `SF` - sign flag. This is set if the result of `CMP` is *signed*, negative.  

So if **PARAM 1** < **PARAM 2**:  
Then **PARAM 1** - **PARAM 2** < 0 and `SF` is set.

And if **PARAM 1** > **PARAM 2**:  
Then **PARAM 1** - **PARAM 2** > 0 and neither `SF` nor `ZF` are set.

And if **PARAM 1** == **PARAM 2**:  
Then **PARAM 1** - **PARAM 2** == 0, and `ZF` is set.

Conditional jumps use these flags in order to determine if they should jump or not. There are 6 conditional jump instructions:
+ `JE` - jump if equal. Jump if `ZF` is set.
+ `JNEQ` - jump if not equal. Jump if `ZF` is not set.
+ `JG` - jump if greater. Jump if neither `ZF` nor `SF` are set.
+ `JGE` - jump if greater or equal. Jump if `SF` is not set or `ZF` is (Although `ZF` will not be set if `SF` is).
+ `JL` - jump if less. Jump if `SF` is set and `ZF` is not.
+ `JLE` - jump if less or equal. Jump if either `SF` or `ZF` are set.

An example of these instructions are as follows:
```
CMP EAX 10
JE is10
JG more10

PUSH EAX
JMP endif

TAG is10
PUSH EBX
JMP endif

TAG more10
CMP EAX 11
JE is11

PUSH EAX
JMP endif

TAG is11
ADD EBX 1
PUSH EBX
JMP endif

TAG endif
; More code
```

Creating a flowchart may help you understand:


```
+-----------+
| EAX == 10 |
+-----------+
 |False |True
 v      +--------->+----------+
 +---------+       | PUSH EBX |--------------------->+
 | EAX > 10|       +----------+                      |
 +---------+                                         |
  |False |True                                       |
  V      +-------->+-----------+                     |
  +----------+     | EAX == 11 |                     |
  | PUSH EAX |     +-----------+                     |
  +----------+      |False |True                     |
        |           V      +--------->+-----------+  |
        |           +----------+      | ADD EBX 1 |  |
        |           | PUSH EAX |      | PUSH EBX  |  |
        |           +----------+      +-----------+  |
        |                |                   |       V
        +<---------------+-------------------+-------+
        V
 +-------------+
 | ; More code |
 +-------------+
```

Now while this flowchart may seem pretty complicated, take a few minutes to try and understand it. Maybe the following C code of the program might help you:

```c
if(EAX == 10){
    push(EBX);
}
else if(EAX > 10){
    if(EAX == 11){
        EBX ++;
        push(EBX);
    }
    else{
        push(EAX);
    }
}
else{
    push(EAX);
}

// More code
```

The reason for the endif tag is so that after each "block statement", the program doesnt continue to the next line. This is basically what the end curly braces do in C.

The only instruction as of now that affects flags is the `CMP` instruction, so you can have code like this:

```
CMP EAX 10
MUL EAX 2
JE is10
ADD EAX 2

TAG is10
DIV EAX
```

In C this is:

```c
EAX *= 2;

if(EAX != 10){
    EAX += 2;
}

EAX /= 2;
```

This basically does:

```c
if(EAX != 10){
    EAX ++;
}
```

***

That's it for now, next up we'll discuss Strudel's version of functions.