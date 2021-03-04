#ifndef DATATYPES_H
#define DATATYPES_H

#include "consts.h"
#include "libs.h"

typedef int bool_t;

typedef enum token_type_e {TOKEN=1, REGISTER, NUM, SIZE, STRING, TAGGEE, FUNCTION}token_type_t;
typedef enum token_e {PUSH=1, POP, MOV, LEA, CMP, JMP, CALL, JE, JNE, JG, JGE, JL, JLE, ADD, SUB, MUL, DIV, TAG, SET, IN, OUT, END}token_t;
typedef enum register_token_e {RIP=NUM_REG_SIZE, RSP, RTP, RBP, RAX, RBX, RCX, RDX}register_token_t;
typedef enum deref_size_e {BYTE=1, WORD, DWORD, QWORD}deref_size_t;
typedef enum function_e {OPEN=1, READ, WRITE, PRNUM}function_t;

typedef struct flags_s{
    unsigned char zf : 1;
    unsigned char sf : 1;
}flags_t;

typedef struct func_flags_s{
    unsigned char print_stack : 1;
    unsigned char print_instructions : 1;
    unsigned char print_regs : 1;
    unsigned char print_flags : 1;
}func_flags_t;

union reg_u{
    long int reg_64;
    int reg_32[2];
    short int reg_16[4];
    char reg_8[8];
};

typedef struct var_s{
    struct var_s * next;

    char * name;
    void * addr;
}var_t;

typedef struct jump_offset_s{
    struct jump_offset_s * next;

    char * tag;
    long int offset;
}jump_offset_t;

struct __attribute__((__packed__)) reg_token_s{
    u_int8_t reg;
    u_int8_t size;
    u_int8_t index;
};

typedef struct registers_s{
    union reg_u rip;
    union reg_u rsp;
    union reg_u rtp;
    union reg_u rbp;
    union reg_u rax;
    union reg_u rbx;
    union reg_u rcx;
    union reg_u rdx;

    union reg_u rx[10];

    int etp;
}registers_t;

typedef struct instruction_s{
    u_int8_t token_type;

    union data{
        u_int8_t token;
        u_int8_t size;
        struct reg_token_s reg;
        long int num;
        char * str;
        u_int8_t function;
    } data;
}instruction_t;

#endif
