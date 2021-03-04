# **LESSON 8**
*Set*

The `SET` instruction is pretty similar to `PUSH`. If you remember [lesson 1](./lesson1.md), I quickly talked about the `data section`. This is a data section meant to be a lot less hectic than the stack. The data section is initialized during compilation, and it's meant to hold strings. It can also hold numbers, though.  

<br>

The `SET` instruction, like `TAG`, only affects the program during compile-time.  
It has the following uses: 

+ `SET` \<VAR NAME> \<NUM>
+ `SET` \<VAR NAME> "\<STRING>"
+ `SET` \<VAR NAME> '\<CHARACTER>'

If you `SET` a number, then the compiler allocates 8 bytes on the data section and sets that to **NUM**. So if you do:
```
SET var 1000
```
The data section will look like (every 2 characters is one byte, represented in hex):
```
e8 03 00 00 00 00 00 00
```
(3e8 is 1000 in hex, so in little endian, it's written like so for a long int.)

If you `SET` a string, the compiler allocates enough room for the length of that string, plus one NUL (0) byte, and sets that to **STRING**. So if I do:
```
SET var "Hello!"
```
It will allocate 7 bytes of memory (Hello! is 6 characters). So the data section will look something like this:
```
48 65 6c 6c 6f 21 00
```

<br >
  
If you `SET` a character, the compiler will allocate only one byte and set that to **CHARACTER**. Like so:
```
SET var 'H'
```
The data section will look like:
```
48
```

<br>

Refrencing **VAR NAME** will give you the address of the variable. This is just the offset of the variable from the beginning of the text section plus the address of the beginning of the text section. So say the text section starts at 0 (which it won't), if I do:
```asm
SET hello_world "Hello!"
SET one_thousand 1000
SET eich 'H'
```
`hello_world` will be 0, `one_thousand` will be 7, and `eich` will 15.  
So, variables are just pointers to the stack.

Strudel also supports escape a few escape codes:
+ `\n` - newline
+ `\r` - carriage return
+ `\b` - backspace
+ `\e` - escape
+ `\\` - is converted to \
