#include "inter.h"

flags_t flags = {0};
bool_t newline = false;
int * stack = NULL;
var_t * text = NULL;
registers_t reg_struct = {0};
instruction_t instructions[INSTRUCTION_SIZE] = {0};
jump_offset_t * jump_offsets[BUFFER_SIZE] = {0};

int init(){
    int return_value = 0;

    stack = mmap(NULL, getpagesize(), PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if(MAP_FAILED == stack){
        return_value = print_error("\e[31mINIT: mmap error\e[0m", -1);
        goto cleanup;
    }

    reg_struct.rsp.reg_64 = (long int)stack;
    reg_struct.rbp.reg_64 = (long int)stack;

    text = mmap(NULL, getpagesize(), PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if(MAP_FAILED == text){
        return_value = print_error("\e[31mINIT: mmap error\e[0m", -1);
        goto cleanup;
    }

cleanup:
    return return_value;
}

void print_help(){
    char * prompt = "\n"
    "\e[32;1mDumDum\e[0m - an educational runtime interpreter with no real goal.\n\n"
    "\e[1mUsage\e[0m:\n"
    "\t\e[1mdum\e[0m [\e[4mOPTION\e[0m]... <\e[4mFILE\e[0m>\n"
    "Interprets FILE\n\n"
    "\e[1mFlags\e[0m\n"
    "\t\e[1m-s\e[0m\tShows the stack after each sequence (after execution)\n"
    "\t\e[1m-r\e[0m\tShows the state of the registers after each sequence (after execution)\n"
    "\t\e[1m-i\e[0m\tShows the parsed instructions after each sequence (before execution)\n"
    "\t\e[1m-f\e[0m\tShows the state of the flags after each sequence (after execution)\n"
    "\t\e[1m-h\e[0m\tShows this text\n";

    puts(prompt);
}
