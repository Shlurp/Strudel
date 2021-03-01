#ifndef VARS_H
#define VARS_H

#include "consts.h"
#include "libs.h"
#include "data_types.h"

extern char * tokens[];
extern char * regs[][5];

extern flags_t flags;
extern bool_t newline;

extern int * stack;
extern char * text;

extern jump_offset_t * jump_offsets[BUFFER_SIZE];
extern var_t * text_vars[BUFFER_SIZE];

extern registers_t reg_struct;
extern instruction_t instructions[INSTRUCTION_SIZE];

extern char * magic;
extern int magic_len;
extern int version[3];
extern int oldest_compatible[3];

#endif
