#include "comp.h"

int * stack = NULL;
char * text = NULL;

char * magic = "STRUDEL";
int magic_len = 0;
int version[3] = {0, 0, 1};
int oldest_compatible[3] = {0, 0, 0};

int init(){
    int return_value = 0;

    magic_len = strnlen(magic, BUFFER_SIZE);

    text = mmap(NULL, getpagesize(), PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if(MAP_FAILED == text){
        return_value = print_error("\e[31mINIT: mmap error\e[0m", -1);
        goto cleanup;
    }

    reg_struct.rtp.reg_64 = (long int)text;

cleanup:
    return return_value;
}
