#include "comp.h"

int main(int argc, char ** argv){
    if(argc < 3){
        printf("\e[31mUsage\e[0m: $ %s <source> <dest>\n", argv[0]);
        goto cleanup;
    }

    init();
    
    compile(argv[1], argv[2]);

cleanup:
    exit(0);
}