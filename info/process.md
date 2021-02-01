This "language" is meant to be  similar to intel's x86 assembly language. It in no way *is* an assembly language, and is meant to be used (and made) for learning purposes.

## **REGISTERS**
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

## **PUSH**
The PUSH instruction has the following usages:
+ **PUSH \<REG>** - Pushes the value stored in the register REG onto the stack (at the stack pointer, RSP)
+ **PUSH \<NUM>** - Pushes the integral value NUM onto the stack (at the stack pointer, RSP)

## **POP**
The POP instruction has two usages:
+ **POP \<REG>** - Pops the top of the stack into the register REG. (Moves the stack pointer, RSP, back)
+ **POP** - Moves the stack pointer back

## **MOV**
The MOV instruction has the following usages:
+ **MOV \<SIZE> \[POINTER1\] \[POINTER2\]** - Moves **SIZE** bytes of data located at POINTER2 to the area pointed to by POINTER1
+ **MOV \<SIZE> \[POINTER1\] \<REG>** - Moves **SIZE** bytes of data from REG into the area pointed to by POINTER1 
+ **MOV \<SIZE> \[POINTER1\] \<NUM>** - Moves **SIZE** bytes of the number NUM into the area pointed to by POINTER1
+ **MOV \<SIZE> \<REG> \[POINTER\]** - Moves **SIZE** bytes of data stored in the area pointed to by POINTER into the register REG
+ **MOV \<REG1> \<REG2>** - Moves the value in REG2 into REG1
+ **MOV \<REG> \<NUM>** - Moves NUM into REG

## **LEA**
The LEA has the following usage:
**LEA \<REG> \[POINTER\]** - Moves the value POINTER (not the value located at) into REG

## **JMP**
Usage:  
**JMP \<TAG>** - Jumps to the line TAG-ged with TAG.  
**At the moment, this instruction is useless, only CALL can be used for functions, and as conditions aren't yet a thing, you can't use this for if statements.**

## **CALL**
Usage:  
**CALL \<TAG>** - Calls a subroutine (pushes RIP, the instruction pointer, onto the stack and then jumps to the TAG)

## **END**
Terminates the program (should only be used at the end of the entry subroutine)