# **LESSON 9**
*Predefined functions*

Strudel has a handful of predefined functions. These allow for file i/o. So using them, you can print and write and read to files.  
Here is a list of the functions:

+ `OPEN`
+ `READ`
+ `WRITE`
+ `PRNUM`

### Open
`OPEN` opens a file. It calls the C's `open` function.  
It takes the following as input:
+ `RAX` - the name of the file to open
+ `RBX` - the number of permission flags to open the file with.
+ The permission flags should be pushed onto the stack. When `OPEN` is called, it pops them off the stack and bitwise ORs them together. The permission flags are the same as the ones in C.

`OPEN` returns the file descriptor of the opened file into `EAX`. On failure, it sets `EAX` to -1.

Here's an example:
```
SET file_name "../../demos/files.asm"

LEA RAX [file_name]
MOV RBX 1
PUSH 2  ; 2 is the value of O_RDWR. The file is opened for reading and writing

CALL OPEN

CMP EAX -1
JE end

; code ...

TAG end
END

```

This will open the file [../../demos/files.asm](../../demos/files.asm) for reading and writing. It checks to see if the open failed.

<br>

### READ
`READ` reads from a file. It calls C's `read` function.  
It takes the following as input:
+ `EAX` - the file descriptor of the file to read from
+ `RBX` - a pointer to the area of memory to fill up from the read
+ `RCX` - the number of bytes to read

`READ` returns the number of bytes read from the file, or -1 on error into `RAX`.

Here's an example, continued from the last one on `OPEN`:
```
MOV RBX RSP
MOV RCX 100

PUSH EAX    ; This is necessary for later
CALL READ
CMP EAX -1
JE end
```

<br>

### WRITE

`WRITE` write to a file. It calls C's `write` function.  
It takes the following as input:
+ `EAX` - the file descriptor of the file to write to
+ `RBX` - a pointer to the area of memory to write from
+ `RCX` - the number of bytes to write

`READ` returns the number of bytes written to the file, or -1 on error into `RAX`.

Here's an example, continued from the last one on `READ`:
```
POP EAX

CALL WRITE
CMP EAX -1
JE end
```

This will just rewrite what we read back into the file, but right after the data. So if it looked like this originally:  
100 bytes of data x ...  
It will look like this afterwards:  
100 bytes of data x 100 bytes of data x ...  

<br>

### PRNUM

`PRNUM` just prints a number (by calling `printf("%li")`)  
It takes just `RAX` as input, it prints `RAX`. It doesn't return or change any registers.

For example:
```
MOV RAX 200
CALL PRNUM
```
This will print 200.

***

Check out [this program](../../demos/atoi.asm). It's just C's `atoi` function rewritten in Strudel.