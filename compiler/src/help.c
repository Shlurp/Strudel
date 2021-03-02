#include "comp.h"

void print_help(){
    char * prompt = "\n"
    "\e[32;1mStrudel\e[0m - an educational language with no real use\n\n"
    "\e[1mUsage\e[0m:\n"
    "\t\e[1mcstru\e[0m [\e[4mOPTION\e[0m]... <\e[4mSRC\e[0m> [\e[4m-o DEST\e[4m]\n"
    "Converts SRC into bytecode, which is stored in DEST\n\n"
    "\e[1mFlags\e[0m\n"
    "\t\e[1m-i\e[0m\tShows the parsed instructions after each sequence (before execution)\n"
    "\t\e[1m-o\e[0m\tSets DEST to the file specified. If this flag is not raised, the bytecode will be stored in a file named a.out\n"
    "\t\e[1m-h\e[0m\tShows this text\n";

    puts(prompt);
}