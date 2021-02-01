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

#define ENTRY_POINT "MAIN"

#define true (1)
#define false (0)

#define NONE (0)

#define FAIL ((void *)(-1))

#define ABS(num) ((num < 0) ? -1 * num : num)

typedef int bool_t;

typedef enum token_type_e {TOKEN=1, REGISTER, NUM, SIZE, STRING}token_type_t;
typedef enum token_e {PUSH=1, POP, MOV, LEA, JMP, CALL, CMP, JE, JNE, JG, JGE, JL, JLE, ADD, SUB, TAG, SET, IN, OUT, END}token_t;
typedef enum register_token_e {RIP=NUM_REG_SIZE, RSP, RBP, RAX, RBX, RCX, RDX}register_token_t;
typedef enum deref_size_e {BYTE=1, WORD, DWORD, QWORD}deref_size_t;
typedef enum flag_e {ZF=1}flag_t;

extern char * tokens[];
extern char * regs[][5];

typedef struct flags_s{
    char print_stack : 1;
    char print_instructions : 1;
    char print_regs : 1;
}flags_t;

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

typedef struct jump_offset_s{
    struct jump_offset_s * next;

    char * tag;
    off_t offset;
}jump_offset_t;

struct reg_token_e{
    register_token_t reg;
    int size;
    int index;
};

typedef struct registers_s{
    union reg_u rip;
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
        char * str;
    } data;
}instruction_t;

extern int FLAGS;
extern bool_t newline;

extern int * stack;
extern var_t * text;

extern jump_offset_t * jump_offsets[BUFFER_SIZE];

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

int get_next_instruction(FILE * source, instruction_t * instruction);

int init();
void print_help();
void print_instructions();
void print_regs();
void print_stack();
int get_curr_line(FILE * source);

unsigned long hash(unsigned char * str);
int insert_jump_offset(char * str, off_t offset);
off_t get_jump_offset(char * str);
off_t find_tag(FILE * source, char * str);

static inline int get_reg(union reg_u ** reg){
    int return_value = 0;

    switch(instructions[reg_struct.etp].data.reg.reg){ 
        case RIP: *reg = &reg_struct.rip; break;
        case RSP: *reg = &reg_struct.rsp; break; 
        case RBP: *reg = &reg_struct.rbp; break;
        case RAX: *reg = &reg_struct.rax; break; 
        case RBX: *reg = &reg_struct.rbx; break; 
        case RCX: *reg = &reg_struct.rcx; break; 
        case RDX: *reg = &reg_struct.rdx; break; 
        default: 
            if(0 <= instructions[reg_struct.etp].data.reg.reg && instructions[reg_struct.etp].data.reg.reg < NUM_REG_SIZE){ 
                *reg = &reg_struct.rx[instructions[reg_struct.etp].data.reg.reg]; 
                break; 
            } 
            else{ 
                printf("\e[31;1mError\e[0m: unexpected register value\n"); 
                return_value = -1; 
                goto cleanup;
            } 
    }

cleanup:
    return return_value;
}

static inline long int get_reg_value(union reg_u * reg, long int * value){
    long int return_value = 0;
    int i = 0;

    i = instructions[reg_struct.etp].data.reg.index;

    switch(instructions[reg_struct.etp].data.reg.size){
        case R_REG_SIZE: *value = reg->reg_64; break;
        case E_REG_SIZE: *value = reg->reg_32[i]; break;
        case X_REG_SIZE: *value = reg->reg_16[i]; break;
        case HL_REG_SIZE: *value = reg->reg_8[i]; break;
        default:
            printf("\e[31mError\e[0m invalid register size");
            return_value = -1;
            goto cleanup;
    }

cleanup:
    return return_value;
}

static inline int set_reg_value(union reg_u * reg, long int value, int i){
    int return_value = 0;
    int index = 0;

    index = instructions[i].data.reg.index;

    switch(instructions[i].data.reg.size){
        case R_REG_SIZE: reg->reg_64 = value; break;
        case E_REG_SIZE: reg->reg_32[index] = value; break;
        case X_REG_SIZE: reg->reg_16[index] = value; break;
        case HL_REG_SIZE: reg->reg_8[index] = value; break;
        default:
            printf("\e[31mError\e[0m invalid register size on line\n");
            return_value = -1;
            goto cleanup;
    }

cleanup:
    return return_value;
}

static inline int rread(void * ptr, size_t size, size_t elements, FILE * source){
    int return_value = 0;

    return_value = fseek(source, reg_struct.rip.reg_64, SEEK_SET);
    if(-1 == return_value){
        goto cleanup;   
    }

    return_value = fread(ptr, size, elements, source);
    
    reg_struct.rip.reg_64 += return_value * size;

cleanup:
    return return_value;
}

static inline off_t rseek(FILE * source, off_t offset, int whence){
    off_t return_value = 0;

    return_value = fseek(source, offset, whence);

    reg_struct.rip.reg_64 = ftell(source);

    return return_value;
}
