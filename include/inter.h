#include "libs.h"
#include "consts.h"
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

int get_next_instruction(FILE * source, instruction_t * instruction);

int init();
void print_help();
void print_instruction(instruction_t instruction);
void print_instructions();
void print_regs();
void print_stack();
void print_flags();
int get_curr_line(FILE * source);

unsigned long hash(unsigned char * str);
int insert_jump_offset(char * str, off_t offset);
off_t get_jump_offset(char * str);
off_t find_tag(FILE * source, char * str);
int jump(FILE * source);
int set_text_var();
void * get_var_value(char * name);

static inline int print_error(char * prompt, int return_value){
    perror(prompt);
    printf("(Errno %i)\n", errno);

    return return_value;
}

static inline int is_whitespace(char c){
    return (c == ' ' || c == '\n');
}

static inline int get_reg(union reg_u ** reg){
    int return_value = 0;

    switch(instructions[reg_struct.etp].data.reg.reg){ 
        case RIP: *reg = &reg_struct.rip; break;
        case RSP: *reg = &reg_struct.rsp; break; 
        case RTP: *reg = &reg_struct.rtp; break;
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

static inline int rgetc(FILE * source){
    char curr_char = -1;
    int error_check = 0;

    error_check = fseek(source, reg_struct.rip.reg_64, SEEK_SET);
    if(-1 == error_check){
        goto cleanup;   
    }

    curr_char = getc(source);
    reg_struct.rip.reg_64 ++;

cleanup:    
    return curr_char;
}

static inline void free_jump_offsets(){
    int i = 0;
    jump_offset_t * node = NULL;
    jump_offset_t * next_node = NULL;

    for(i=0; i<BUFFER_SIZE; i++){
        node = jump_offsets[i];

        while(NULL != node){
            next_node = node->next;
            if(NULL != node->tag){
                free(node->tag);
            }
            free(node);
            node = next_node;
        }
    }
}

static inline void free_vars(){
    int i = 0;
    var_t * node = NULL;
    var_t * next = NULL;

    for(i=0; i<BUFFER_SIZE; i++){
        node = text_vars[i];

        while(NULL != node){
            next = node->next;

            if(NULL != node->name){
                free(node->name);
            }
            free(node);

            node = next;
        }
    }
}
