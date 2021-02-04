# **LESSON 1**  
*Data storage basics*

<br>

In these "lessons", I will attempt to explain to newcomers how to use Dumdum and how the internals operate.  
First, it's important to understand the three methods of data storage:

+ **Registers** - Registers are fixed-sized areas of memory (in reality they're stored in devices, Dumdum's registers simulate a CPU's registers) that are quick to access. They are the "final stage" of memory: you should only move values into registers when you need them. Registers should be changing constantly, if you need long-term memory, use one of the following:

+ **The Stack** - The stack is an area of memory that provides quick and simple memory access. The stack allows you to push and pop values off of it, along with random access. It is for the most part what is responsible for the scoping of your functions.

+ **The Text Data Section** - Constant values like strings and constants are stored here usually, but Dumdum allows these values to be mutable. The primary purpose of the Text section in Dumdum is to allow you to create strings.

*** 

## **Registers**

Registers are slightly more complicated then your average var. Each register is actually several registers. A diagram of a Dumdum register looks like:

```
                    R*X
   _______________________________________
  /       E*X                             \
  | __________________                    |
  |/   *X             \                   |
  || _______          |                   |
  ||/       \         |                   |
  +----+----+----+----+----+----+----+----+
  | *L | *H |    |    |    |    |    |    |
  |    |    |    |    |    |    |    |    |
  +----+----+----+----+----+----+----+----+
```
(The * is replaced depending on the register, for example R**A**X)  
Each tile is one byte.  
There are several reasons for this structure: backwards compatability, quick indexing, and more. The reason Dumdum uses it is because Intel does, and Dumdum is just meant to be a simple Assembly-similar language.  
The following are all of the registers with this structure:
+ `RAX`
+ `RBX`
+ `RCX`
+ `RDX`

In addition to these registers, there are 10 more, but these don't have the structure discussed above. They are all only 64 bit.
+ `r0`
+ `r1`
+ `r2`  
...
+ `r9`

All of the above registers are for you to use at your leisure. The following should only hold specific values. Like the registers previously discussed, they are also 64 bit.

+ `RIP` - The instruction pointer. (For now it stores the current offset in the file, in the future Dumdum will hopefully compile into a bytecode, and it will act more like the normal RIP and will store the index of the opcode.) Do not alter this register with arithmetic operations. When you should alter it will be discussed in the future.
+ `RSP` - The stack pointer. This points to the top (current position) of the stack. What this means exactly will be discussed later.
+ `RTP` - The text data section pointer. This points to the top of the text data section. Do not alter this register at all.
+ `RBP` - The stack frame pointer. This register isn't really special. The only thing that makes it different than say, `r0`, is that it is intialized with the same value as `RSP`. This register can be used to provide scoping for your functions.

***

## **The Stack**

The stack is essentially just a large buffer. Every element in the stack is a `long int` (8 bytes). We will discuss what exactly this means later. The stack only holds numerical values. There are ways to push strings onto it, but the process to do so is a bit complicated and just plain annoying.

***

## **The Text Data Section**

The Text data section (from now on, Text or Text section) is quite similar to the stack, but it isn't meant to be popped. Its main purpose is to hold global variables and strings.