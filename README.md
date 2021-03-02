# **STRUDEL**

Strudel is an educational (for me) language. It's purpose is to be a language whose syntax is similar to assembly. It is compiled into bytecode and then run by an interpreter.

## **Dependencies**
+ **Linux** - the program uses some linux-specific system calls, and most probably won't work on any other OS. I don't have any other OS, so maybe it does.
+ **64 bit architecture** - I'm not entirely sure what will happen if you don't use a 64 bit arch.
+ **gcc** - I used `make` instead of `cmake`, so you're going to need `gcc`.
+ (**A terminal that supports ANSI escape codes** - this isn't exactly necessary, I don't think. You can check this by going onto your terminal and executing: 
    ```bash
    $ echo -e "\e[31;1mThis text should be red and bold\e[0m"
    ```
    The text should be red and bold. If it's not, Strudel's error messages may look weird.)

## **Downloading and building**
First, clone the repository. Then you'll need to compile the program:
```sh
$ git clone https://github.com/Shlurp/Strudel
$ cd Strudel
$ make
```
This will:
+ Create clone this repository into a directory named `Strudel`.
+ Enter that directory
+ Compile the program

You will get two compiled programs: `stru` and `cstru`.   
+ `cstru` is the compiler for Strudel. It converts Strudel source code into bytecode.  
+ `stru` executes the bytecode compiled by `cstru`.
To check if they are working correctly, execute the following:
```sh
$ ./cstru demos/hello_world.asm
$ ./stru a.out
```
You should get a bright bold yellow message "Hello World!". If you don't, check that you have all of [the dependencies](#dependencies).

## **Documentation**
The quick documentation of Strudel can be found [here](./info/info.md#strudel)  
More in depth tutorials can be found [here](./info/tutorial/lesson1.md#lesson-1)