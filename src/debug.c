#include "inter.h"

int FLAGS = 0;
int line = 0;
bool_t newline = false;
int * stack = NULL;
var_t * text = NULL;
registers_t register_struct = {0};
instruction_t instructions[INSTRUCTION_SIZE] = {0};

int init(){
    int return_value = 0;

    stack = mmap(NULL, getpagesize(), PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if(MAP_FAILED == stack){
        return_value = print_error("\e[31mINIT: mmap error\e[0m", -1);
        goto cleanup;
    }

    register_struct.rsp = (long int)stack;
    register_struct.rbp = (long int)stack;

    text = mmap(NULL, getpagesize(), PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if(MAP_FAILED == text){
        return_value = print_error("\e[31mINIT: mmap error\e[0m", -1);
        goto cleanup;
    }

cleanup:
    return return_value;
}

void print_instructions(){
    int i=0;

    for(i=0; i<INSTRUCTION_SIZE && instructions[i].token_type != NONE; i++){
        fputs("TOKEN TYPE: ", stdout);
        switch(instructions[i].token_type){
            case TOKEN: 
                puts("TOKEN");
                break;
            case REGISTER:
                puts("REGISTER");
                break;
            case NUM:
                puts("NUM");
                break;
            case POINTER:
                puts("POINTER");
                break;

            default:
                puts("\n\e[31mINVALID TOKEN TYPE\e[0m");
                goto cleanup;
        }

        if(TOKEN == instructions[i].token_type){
            fputs("TOKEN: ", stdout);

            switch(instructions[i].data.token){
                case PUSH:
                    puts("PUSH");
                    break;
                case POP:
                    puts("POP");
                    break;
                case MOV:
                    puts("MOV");
                    break;
                case IN:
                    puts("[");
                    break;
                case OUT:
                    puts("]");
                    break;
                case ADD:
                    puts("ADD");
                    break;
                case SUB:
                    puts("SUB");
                    break;
                default:
                    puts("\n\e[31mINVALID TOKEN\e[0m");
                    goto cleanup; 
            }
        }
        else if(REGISTER == instructions[i].token_type){
            fputs("REGISTER: ", stdout);

            switch(instructions[i].data.reg){
                case RSP:
                    puts("RSP");
                    break;
                case RBP:
                    puts("RBP");
                    break;
                case RAX:
                    puts("RAX");
                    break;
                case RCX:
                    puts("RCX");
                    break;
                default:
                    if(0 <= instructions[i].data.reg && instructions[i].data.reg < NUM_REG_SIZE){
                        printf("r%i\n", instructions[i].data.reg);
                    }
                    else{
                        puts("\n\e[31mINVALID REGISTER\e[0m");
                        goto cleanup;
                    }
            }
        }
        else{
            printf("DATA: %i\n", instructions[i].data.num);
        }

        putchar('\n');
    }

cleanup:
    return;
}

void print_regs(){
    int i = 0;

    printf("\e[1mRSP\e[0m: %li | \e[1mRBP\e[0m: %li | \e[1mRAX\e[0m: %li | \e[1mRCX\e[0m: %li\n", register_struct.rsp, register_struct.rbp, register_struct.rax, register_struct.rcx);

    for(i=0; i<NUM_REG_SIZE; i++){
        printf("\e[1mr%i\e[0m: %li | ", i, register_struct.rx[i]);
    }

    putchar('\n');
}