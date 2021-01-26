#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>

#define STACK_SIZE (2048)
#define TEXT_SIZE (1024)
#define BUFFER_SIZE (512)
#define INSTRUCTION_SIZE (16)
#define NUM_REG_SIZE (10)

#define true (1)
#define false (0)

#define NONE (0)

#define FAIL ((void *)(-1))

typedef int bool_t;

typedef enum token_type_e {TOKEN=1, REGISTER, NUM, POINTER}token_type_t;
typedef enum token_e {PUSH=1, POP, MOV, JMP, CMP, JE, JNE, JG, JGE, JL, JLE, ADD, SUB, SET, IN, OUT}token_t;
typedef enum register_token_e {RSP=NUM_REG_SIZE, RBP, RAX, RCX}register_token_t;
typedef enum flag_e {ZF=1}flag_t;

extern char * tokens[];
extern char * regs[];

typedef struct var_s{
    struct var_s * next;

    void * value;
}var_t;

typedef struct registers_s{
    long int rsp;
    long int rbp;
    long int rax;
    long int rcx;

    long int rx[10];

    int etp;
}registers_t;

typedef struct instruction_s{
    token_type_t token_type;

    union data{
        token_t token;
        register_token_t reg;
        int num;
        void * pointer;
    } data;
}instruction_t;

extern int FLAGS;
extern int line;
extern bool_t newline;

extern int * stack;
extern var_t * text;

extern registers_t register_struct;
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