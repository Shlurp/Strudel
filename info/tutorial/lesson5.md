# **LESSON 5**
*TAGs, jumps, and calls*

One of the most important parts of many languages is the ability to create conditions and functions. While most of the time these two features are seperate, in Strudel, they are very closely linked. Despite this, these two features have nuances that must be discussed in more detail; they cannot be done justice in one file.

First off, the feature that makes both functions and conditions work is the `TAG`. A `TAG` saves the current offset in a file under a referenceable name. It has one use:  
`TAG` \<Tag name>  
Now referencing the tag **Tag name** will give you the offset of the (compiled) file at this point. (You cannot `MOV` or `LEA` with a `TAG`, though.) Tags are global, and the tag will point to the where the last `TAG` is. 

Next, we have the `JMP` instruction. It has one use:  
`JMP` \<Tag>  
It sets the current offset in the file to the offset that is represented by **Tag name**. This jump is unconditional, if the program reaches a `JMP` instruction, it will be executed regardless of the current state of the program. This is in contrast to the other types of jumps we will discuss later.  
An example of a program with `TAG`s and `JMP`s is:

```
TAG open

PUSH EAX
ADD EAX 1

JMP open

TAG close
```

This program creates a tag, **open**, pushes `EAX` onto the stack, increments it by 1, and then jumps to **open**. In C it could be written like so (this is the more literal interpretation):
```c
open:

push(eax);   // We are assuming that push is some function
eax ++;

goto open;
end:

```

A more understandable program would be:
```c
do{
    push(eax);
    eax ++;
}while(1);
```

There is no condition here, so the do-while could instead be a while.

***

So far, this shouldn't be anything new. Labels (tags) and goto statements (jmps) are already a thing in C.
