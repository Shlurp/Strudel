#include "inter.h"

int FLAGS = 0;
int line = 0;
bool_t newline = false;
int * stack = NULL;
var_t * text = NULL;
registers_t reg_struct = {0};
instruction_t instructions[INSTRUCTION_SIZE] = {0};

int init(){
    int return_value = 0;

    stack = mmap(NULL, getpagesize(), PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if(MAP_FAILED == stack){
        return_value = print_error("\e[31mINIT: mmap error\e[0m", -1);
        goto cleanup;
    }

    reg_struct.rsp.reg_64 = (long int)stack;
    reg_struct.rbp.reg_64 = (long int)stack;

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
            case SIZE:
                puts("SIZE");
                break;

            default:
                puts("\n\e[31mINVALID TOKEN TYPE\e[0m");
                goto cleanup;
        }

        if(TOKEN == instructions[i].token_type){
            fputs("TOKEN: ", stdout);

            puts(tokens[instructions[i].data.token-1]);
        }
        else if(SIZE == instructions[i].token_type){
            fputs("SIZE: ", stdout);

            switch(instructions[i].data.size){
                case BYTE: puts("BYTE"); break;
                case WORD: puts("WORD"); break;
                case DWORD: puts("DWORD"); break;
                case QWORD: puts("QWORD"); break;
                default: puts("\e[31mINVALID SIZE\e[0m"); break;
            }
        }
        else if(REGISTER == instructions[i].token_type){
            fputs("REGISTER: ", stdout);

            switch(instructions[i].data.reg.reg){
                case RSP:
                    puts("RSP");
                    break;
                case RBP:
                    puts("RBP");
                    break;
                case RAX:
                    puts("RAX");
                    break;
                case RBX:
                    puts("RBX");
                    break;
                case RCX:
                    puts("RCX");
                    break;
                case RDX:
                    puts("RDX");
                    break;
                default:
                    if(0 <= instructions[i].data.reg.reg && instructions[i].data.reg.reg < NUM_REG_SIZE){
                        printf("r%i\n", instructions[i].data.reg.reg);
                    }
                    else{
                        puts("\n\e[31mINVALID REGISTER\e[0m");
                        goto cleanup;
                    }
            }
        }
        else{
            printf("DATA: %li\n", instructions[i].data.num);
        }

        putchar('\n');
    }

cleanup:
    return;
}

void print_regs(){
    int i = 0;

    printf("\e[1mRSP\e[0m: %li | \e[1mRBP\e[0m: %li | \e[1mRAX\e[0m: %li | \e[1mRBX\e[0m: %li| \e[1mRCX\e[0m: %li| \e[1mRDX\e[0m: %li\n", reg_struct.rsp.reg_64, reg_struct.rbp.reg_64, reg_struct.rax.reg_64, reg_struct.rbx.reg_64, reg_struct.rcx.reg_64, reg_struct.rdx.reg_64);

    for(i=0; i<NUM_REG_SIZE; i++){
        printf("\e[1mr%i\e[0m: %li | ", i, reg_struct.rx[i].reg_64);
    }

    putchar('\n');
}
