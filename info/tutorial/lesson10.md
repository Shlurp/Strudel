# **LESSON 10**
*Global variables and tags*

Strudel allows for you to define variables and tags across multiple files and compile them together. This allows you to share tags and variables across multiple files.  
But not all tags and variables *have* to be shared. Let's say you have a tag `open` that's the opening tag for a loop in one file, and you don't want it to be written by another file, and you won't ever need to use that tag in the other file.  

For instance, let's say you have the file `is_odd`:

```
TAG is_odd          ; is_odd function:
                    ; Parameters:
                    ; RBX - number to check for even-ness
                    ; Returns:
                    ; 1 if odd, 0 if even (in EAX)

    MOV EAX RBX     ; Move RBX (the value to check) into EAX

    AND EAX 1       ; AND EAX with 1. 
                    ; If EAX is odd, then its least suignificant bit is 1. So if EAX (RBX) is odd, then EAX will now equal 1

POP RIP
```

Now, let's say you have another file, `main`, which looks like so:

```
SET even_message "The number is even!\n"
SET odd_message "The number is odd!\n"
SET data_end '0'

TAG MAIN

    MOV RBX 10

    CALL is_odd
    CMP EAX 1
    MOV EAX 1
    JE num_is_odd

        LEA RBX [even_message]
        LEA RCX [odd_message - even_message]
        
        CALL WRITE
    JMP end

    TAG num_is_odd
        LEA RBX [odd_message]
        LEA RCX [data_end - odd_message]

        CALL WRITE

TAG end
END
```

Essentially, all this does is print `This number is even!` if RBX is even (which it is in this case) and `This number is odd!` if it's odd.  
But you may have noticed that `is_odd` is not defined in `main`. So what you need to do is compile it like so:
```sh
$ ./cstru main is_odd
```
You should get the error: `Error: Variable is_odd referenced but never declared`. This is a linker error, this is because we never declared `is_odd` as a `GLOBAL` function. So the file `main` can't access it. All we need to do is fix it like so (in `is_odd` file):

```
TAG GLOBAL is_odd   ; is_odd function:
                    ; Parameters:
                    ; RBX - number to check for even-ness
                    ; Returns:
                    ; 1 if odd, 0 if even (in EAX)

    MOV EAX RBX     ; Move RBX (the value to check) into EAX

    AND EAX 1       ; AND EAX with 1. 
                    ; If EAX is odd, then its least suignificant bit is 1. So if EAX (RBX) is odd, then EAX will now equal 1

POP RIP
```

Now that the tag `is_odd` is declared as a GLOBAL tag, it can now be accessed by other files it's compiled with.

The same goes for variables set with `SET`. You can write `SET GLOBAL`... and that variable can be accessed by any file compiled with the file the `SET GLOBAL` is in. The only thing you must be aware of is that if the variable is set with just `SET` (it's not global) it still exists in the data section, and it can still be accessed by other files, just not with its name.