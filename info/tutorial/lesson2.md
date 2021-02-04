# **LESSON 2**  
*Push and Pop*

<br>

The two most important instructions when dealing with the stack are `PUSH` and `POP`. These relativley simple instructions can relieve you of a lot of headaches.

*** 

## **PUSH** 

As discussed earlier, the `RSP` register stores the address of the top of the stack (in reality, the stack grows downwards, but in Dumdum, it grows upwards). But what constitutes as the "top" of the stack? Is the end of the memory allocated? Well, really the top of the stack is defined by the last `PUSH` or `POP`. Allow me to explain:  
When you `PUSH` a value onto the stack, you set the value at `RSP` to whatever you're pushing, and then it increments `RSP` by 8 (the size of the elements on the stack).  
Let's say this is what your stack looked like:

```
> 1  
> 2
> 3 < RSP
```

Obviously something is fishy with this stack. This becomes apparent when you `PUSH` some value onto the stack. Let's say we `PUSH` 4. The value at RSP is set to 4, so the stack looks like:

```
> 1  
> 2
> 4 < RSP
```

So you can see that something happened so a value on the stack was pushed and then RSP was moved back.  
After setting the value at `RSP`, `RSP` is incremented by 8, so the stack now looks like:

```
> 1  
> 2
> 4
> 0 < RSP
```

Now, `PUSH` takes one parameter: the value to push. This can either be a numerical value (eg. 4) or a register (eg. RAX). So if you have the instruction:  
`PUSH` **RAX**  
And `RAX` is say, 314, and the stack looks like:

```
> 2718
> 6282
> 626
> 0 < RSP
```

The resulting stack will look like:

```
> 2718
> 6282
> 626
> 314
> 0 < RSP
```

***

## **POP**

The `POP` instruction is basically the opposite of the `PUSH` instruction; it decrements `RSP` by 8, then it sets an input register to the value at the top of the stack (where `RSP` was moved to).

For instance, let's say you have a stack that looks like:

```
> 121
> 234
> 256
> 989
> 0 < RSP
```

And you do:  
`POP` **RAX**  
`RSP` is moved back:

```
> 121
> 234
> 256
> 989 < RSP
```

And the value is moved into `RAX`, so `RAX` now equals 989.  
**POPping does not change the value that is popped off, so the resulting stack will be:**
```
> 121
> 234
> 256
> 989 < RSP
```

**And not:**

```
> 121
> 234
> 256
> 0 < RSP
```

***

That's it for pushing and popping. Their uses and benefits will become a lot more clear in the future.