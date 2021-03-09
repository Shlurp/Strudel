#ifndef COMP_GLOBALS_H
#define COMP_GLOBALS_H

#include "consts.h"

#include "data_types.h"

extern variable_t * variables[BUFFER_SIZE];
char * functions[4];
extern char * obj_magic;
extern int obj_magic_len;
extern char * sobj_magic;
extern int sobj_magic_len;

#endif