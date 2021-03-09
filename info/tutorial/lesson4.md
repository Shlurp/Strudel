# **LESSON 4**
*LEA and arithmetic operations*

`LEA` is your friend. `LEA` stands for "load effective address", and while this name may make it sound like a complicated instruction, it may be one of the easiest to understand adn use, probably.  
While `MOV` moves the value at a pointer (among its other uses) into some other area of memory, `LEA` moves the value *of* that pointer into the area of memory. This makes it extremely useful. It can be used for more complicated arithmeticoperations, like: 4 + `RAX` + `RBX`, and for getting the address of a variable (which we will discusss later).  
`LEA` has one use:  
`LEA` \<REG> \[POINTER\]  
And all it does is move the value of **POINTER** into **REG**. Heck, **POINTER** doesn't even need to actually point to anything. For example:  

```
MOV RAX 1
MOV RBX 2
LEA RCX [RAX + RBX]
```

`RCX` will now be equal to 3. 

**AS OF NOW, LEA doesn't support multiplication or division, as Strudel doesn't support the order of operations**

Now, while `LEA` may not support all arithmetic operations, there are instructions that do:
+ `ADD` \<REG 1> \<REG 2 / NUM> - Adds **REG 2** or **NUM** to **REG 1**
+ `SUB` \<REG 1> \<REG 2 / NUM> - Subtracts **REG 2** or **NUM** from **REG 1**
+ `MUL` \<REG 1> \<REG 2 / NUM> - Multiplies **REG 1** by **REG 2** or **NUM**
+ `DIV` \<REG 1> \<REG 2 / NUM> - Divides **REG 1** by **REG 2** or **NUM** 

There are also bitwise operators:
+ `AND`\<REG 1> \<REG 2 / NUM> - Bitwise ANDs **REG 2** or **NUM** to **REG 1**
+ `OR`\<REG 1> \<REG 2 / NUM> - Bitwise ORs **REG 2** or **NUM** to **REG 1**
+ `XOR`\<REG 1> \<REG 2 / NUM> - Bitwise XORs **REG 2** or **NUM** to **REG 1**
***

For a demo of how these five instruction work, look [here](../../demos/tutorials/lea.asm)