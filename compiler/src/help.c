#include "comp.h"

void print_help(){
    char * prompt = "\n"
    "\e[32;1mStrudel\e[0m - an educational language with no real use\n\n"
    "\e[1mUsage\e[0m:\n"
    "\t\e[1mcstru\e[0m [\e[4mOPTION\e[0m]... <\e[4mSRC\e[0m>... [\e[4m-o DEST\e[0m]\n"
    "Converts SRC into bytecode, which is stored in DEST\n\n"
    "\e[1mFlags\e[0m\n"
    "\t\e[1m-i\e[0m\tShows the parsed instructions after each sequence (before execution)\n"
    "\t\e[1m-o\e[0m\tSets DEST to the file specified. If this flag is not raised, the bytecode will be stored in a file named stru.out\n"
    "\t\e[1m-c\e[0m\tCompiles file SRC (only one) without linking into Strudel object files. These cannot be executable\n"
    "\t\e[1m-l\e[0m\tLinks Strudel object files SRC... into a file that can be executed by the Strudel interpreter\n"
    "\t\e[1m-h\e[0m\tShows this text\n";

    puts(prompt);
}