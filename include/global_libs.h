#ifndef GLOBALS_H
#define GLOBALS_H

#include "libs.h"
#include "consts.h"
#include "data_types.h"
#include "consts.h"
#include "vars.h"

static inline int print_error(char * prompt, int return_value){
    perror(prompt);
    printf("(Errno %i)\n", errno);

    return return_value;
}

static inline void * ptr_print_error(char * prompt, void * return_value){
    perror(prompt);
    printf("(Errno %i)\n", errno);

    return return_value;
}

#endif