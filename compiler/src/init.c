#include "comp.h"

int * stack = NULL;
char * text = NULL;

char * magic = "STRUDEL";
char * obj_magic = "OBJSTRUDEL";
char * sobj_magic = "SHAREDOBJSTRUDEL";
int obj_magic_len = 0;
int magic_len = 0;
int sobj_magic_len = 0;
int version[3] = {0, 1, 1};
int oldest_compatible[3] = {0, 0, 0};

int page_size = 0;

int init(){
    int return_value = 0;

    magic_len = strnlen(magic, BUFFER_SIZE);
    obj_magic_len = strnlen(obj_magic, BUFFER_SIZE);
    sobj_magic_len = strnlen(sobj_magic, BUFFER_SIZE);

    page_size = getpagesize();

    text = mmap(NULL, page_size, PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if(MAP_FAILED == text){
        return_value = print_error("\e[31mINIT: mmap error\e[0m", -1);
        goto cleanup;
    }

    reg_struct.rtp.reg_64 = (long int)text;

cleanup:
    return return_value;
}
