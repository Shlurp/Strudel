#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG_STR "\e[31mDEBUG\e[0m"
#define DEBUG (puts(DEBUG_STR));

#define STACK_SIZE (2048)
#define TEXT_SIZE (1024)
#define BUFFER_SIZE (512)
#define INSTRUCTION_SIZE (16)
#define NUM_REG_SIZE (10)
#define STACK_ELEMENT_SIZE (8)

#define R_REG_SIZE (64)
#define E_REG_SIZE (32)
#define X_REG_SIZE (16)
#define HL_REG_SIZE (8)

#define true (1)
#define false (0)

#define NONE (0)

#define FAIL ((void *)(-1))

typedef int bool_t;

typedef enum token_type_e {TOKEN=1, REGISTER, NUM, SIZE}token_type_t;
typedef enum token_e {PUSH=1, POP, MOV, JMP, CMP, JE, JNE, JG, JGE, JL, JLE, ADD, SUB, SET, IN, OUT}token_t;
typedef enum register_token_e {RSP=NUM_REG_SIZE, RBP, RAX, RBX, RCX, RDX}register_token_t;
typedef enum deref_size_e {BYTE=1, WORD, DWORD, QWORD}deref_size_t;
typedef enum flag_e {ZF=1}flag_t;

extern char * tokens[];
extern char * regs[][5];

union reg_u{
    long int reg_64;
    int reg_32[2];
    short int reg_16[4];
    char reg_8[8];
};

typedef struct var_s{
    struct var_s * next;

    void * value;
}var_t;

struct reg_token_e{
    register_token_t reg;
    int size;
    int index;
};

typedef struct registers_s{
    union reg_u rsp;
    union reg_u rbp;
    union reg_u rax;
    union reg_u rbx;
    union reg_u rcx;
    union reg_u rdx;

    union reg_u rx[10];

    int etp;
}registers_t;

typedef struct instruction_s{
    token_type_t token_type;

    union data{
        token_t token;
        deref_size_t size;
        struct reg_token_e reg;
        long int num;
    } data;
}instruction_t;

extern int FLAGS;
extern int line;
extern bool_t newline;

extern int * stack;
extern var_t * text;

extern registers_t reg_struct;
extern instruction_t instructions[INSTRUCTION_SIZE];

static inline int print_error(char * prompt, int return_value){
    perror(prompt);
    printf("(Errno %i)\n", errno);

    return return_value;
}

static inline int is_whitespace(char c){
    return (c == ' ' || c == '\n');
}

int init();
void print_instructions();
void print_regs();