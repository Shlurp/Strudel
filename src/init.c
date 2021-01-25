#include "inter.h"

int FLAGS = 0;
int line = 0;
bool_t newline = false;
int * stack = NULL;
var_t * text = NULL;
registers_t register_struct = {0};
instruction_t instructions[INSTRUCTION_SIZE] = {0};

int init(){
    int return_value = 0;

    stack = mmap(NULL, getpagesize(), PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if(MAP_FAILED == stack){
        return_value = print_error("\e[31mINIT: mmap error\e[0m", -1);
        goto cleanup;
    }

    register_struct.rsp = (long int)stack;
    register_struct.rbp = (long int)stack;

    text = mmap(NULL, getpagesize(), PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if(MAP_FAILED == text){
        return_value = print_error("\e[31mINIT: mmap error\e[0m", -1);
        goto cleanup;
    }

cleanup:
    return return_value;
}