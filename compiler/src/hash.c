#include "comp.h"

/**
 * djb2 hash algorithm by Dan Bernstein: http://www.cse.yorku.ca/~oz/hash.html
 */
unsigned long hash(unsigned char * str){
    unsigned long hash = 5381;
    int c = 0;

    do{
        c = *(str++);
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }while(c != 0);
    return hash;
}
