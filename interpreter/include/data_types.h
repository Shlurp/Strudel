#ifndef INT_DATATYPES_H
#define INT_DATATYPES_H

typedef struct func_flags_s{
    unsigned char print_stack : 1;
    unsigned char print_instructions : 1;
    unsigned char print_regs : 1;
    unsigned char print_flags : 1;
}func_flags_t;

#endif