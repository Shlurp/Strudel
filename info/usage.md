# **DUMDUM**

This "language" is meant to be  similar to intel's x86 assembly language. It in no way *is* an assembly language, and is meant to be used (and made) for learning purposes.

### **REGISTERS**
Registers are a union defined as:
```c
typedef union reg_u{
    long int reg_64;
    int reg_32[2];
    short reg_16[4];
    char reg_8[8];
}reg_t;
```

This is in order to allow for access of sub-registers (EAX, AX, AL, AH, etc.) and preserve the ability to access all registers.

<br>

***
## **INSTRUCTIONS**
***
### **PUSH**
The PUSH instruction has the following usages:
+ **PUSH \<REG>** - Pushes the value stored in the register REG onto the stack (at the stack pointer, RSP)
+ **PUSH \<NUM>** - Pushes the integral value NUM onto the stack (at the stack pointer, RSP)

### **POP**
The POP instruction has two usages:
+ **POP \<REG>** - Pops the top of the stack into the register REG. (Moves the stack pointer, RSP, back)
+ **POP** - Moves the stack pointer back

### **ADD**
The ADD instruction has the following usage:  
**ADD \<REG1> \<REG2 / NUM>** - This increments REG1 by the value of REG2 or NUM.

### **SUB**
The SUB instruction has the following usage:  
**SUB \<REG1> \<REG2 / NUM>** - This decrements REG1 by the value of REG2 or NUM.

### **MOV**
The MOV instruction has the following usages:
+ **MOV \<SIZE> \[POINTER1\] \[POINTER2\]** - Moves **SIZE** bytes of data located at POINTER2 to the area pointed to by POINTER1
+ **MOV \<SIZE> \[POINTER1\] \<REG>** - Moves **SIZE** bytes of data from REG into the area pointed to by POINTER1 
+ **MOV \<SIZE> \[POINTER1\] \<NUM>** - Moves **SIZE** bytes of the number NUM into the area pointed to by POINTER1
+ **MOV \<SIZE> \<REG> \[POINTER\]** - Moves **SIZE** bytes of data stored in the area pointed to by POINTER into the register REG
+ **MOV \<REG1> \<REG2>** - Moves the value in REG2 into REG1
+ **MOV \<REG> \<NUM>** - Moves NUM into REG

### **LEA**
The LEA has the following usage:
**LEA \<REG> \[POINTER\]** - Moves the value POINTER (not the value located at) into REG

### **TAG**
Usage:
**TAG \<NAME>**  
This creates a tag by the name of NAME, and jumps to this tag will jump to the line underneath this instruction.

### **CMP**
The CMP function compares two values, and has the following usage:  
**CMP \<REG / NUM> \<REG / NUM>**  
The function then subtracts the first parameter from the second, and sets the zero flag (zf) and signed flag (sf) accordingly.  
The zf is set if the result of the subtraction is 0, and the sf is set if the result of the operation is negative.

### **JUMPS**
There are two types of jumps:  
Uncoditional jumps, jumps that jump no matter what:  
**JMP \<TAG>** - Jumps to the line TAG-ged with TAG.  

Conditional jumps, jumps that jump only if their condition is met:
+ **JL \<TAG>** - Jumps to TAG if the signed flag (sf) is set and the zero flag (zf)isn't
+ **JG \<TAG>** - Jumps to TAG if neither zf nor sf are set
+ **JLE \<TAG>** - Jumps to TAG if zf or sf is set
+ **JGE \<TAG>** - Jumps to TAG if sf isn't set or zf is
+ **JE \<TAG>** - Jumps to TAG if zf is set


### **CALL**
Usage:  
**CALL \<TAG>** - Calls a subroutine (pushes RIP, the instruction pointer, onto the stack and then jumps to the TAG)

### **SET**
Usage:
**SET \<VAR NAME> \<STRING / NUM>** - Creates a global variable (in the text data section) by the name of NAME with the value STRING or NUM. You must use quotation marks for a string.  
The following are recognized escape sequences:
+ `\n` - newline
+ `\r` - carriage return
+ `\b` - backspace
+ `\e` - escape character

### **END**
Terminates the program (should only be used at the end of the entry subroutine)  

<br>

***
## **INFORMATION**
***

### **IMPORTANT TAGS**  
  
<br>

**The ENTRY Tag**  
The entrypoint is a tag named "MAIN". This is where the program begins. No code before it will be executed (unless it is jumped/called to)  

For example:
```asm
PUSH 100
TAG MAIN
PUSH 200
```

The stack after execution will look like:
```
> 200
```

<br>

***

**The END Tag**  
The end point is a tag named "END" that terminates the program. No code after it will be executed (unless it is jumped to).

For example:
```asm
PUSH 100
TAG MAIN
PUSH 200
END
PUSH 300
```
The stack after execution will look like:
```
> 200
```

***
<br>

### **FUNCTIONS**
The following are all of the predefined functions:
<br>

**WRITE**  
The write function calls C's standard `write` function.
+ **PARAMS**:
    + EAX - the file descriptor to write to
    + RBX - a pointer to the data to write
    + RCX - the number of bytes to write
+ **RETURNS**: the number of bytes written into EAX.

***
<br>

### **REGISTERS**

<br>

**RIP**  
The RIP register is the instruction pointer register. It holds the current offset in the file that the interpreter is reading from.

<br>

***

**RSP**  
The RSP register is the stack pointer register. It is incrememnted after every PUSH and decremented after every POP. PUSH'd values will go onto the stack at the address stored in RSP. When a value is POP'd off of the stack, the value taken is the element before RSP.  
For example:

```
> 10
> 20
> 30
> 40
> 50 < RSP
```

POPping a value off the stack will give the value 40. And will result in:

```
> 10
> 20
> 30
> 40 < RSP
> 50 
```

PUSHing the value 60 (we haven't POPped) will result in:

```
> 10
> 20
> 30
> 40
> 60 
> 0  < RSP
```

This behavior could be simulated by using a MOV and LEA/(ADD/SUB) instruction on a different register.

<br>

***

**RBP**  
The register RBP is the frame pointer register. When the program first starts, it and RSP have the same value. It is implicitly changed by any instruction. It may only be explicitly changed.   
Its use is to create stack frames, allowing for the simulation of scopes.  
Unlike RIP and RSP, it is not really a special register, but it should only be used for stack-frame purposes. 