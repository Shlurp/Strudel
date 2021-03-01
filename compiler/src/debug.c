#include "comp.h"

void print_instruction(instruction_t instruction){
    fputs("TOKEN TYPE: ", stdout);
    switch(instruction.token_type){
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
        case STRING:
            puts("STRING");
            break;

        default:
            puts("\n\e[31mINVALID TOKEN TYPE\e[0m");
            goto cleanup;
    }

    if(TOKEN == instruction.token_type){
        fputs("    TOKEN: ", stdout);

        puts(tokens[instruction.data.token-1]);
    }
    else if(SIZE == instruction.token_type){
        fputs("    SIZE: ", stdout);

        switch(instruction.data.size){
            case BYTE: puts("BYTE"); break;
            case WORD: puts("WORD"); break;
            case DWORD: puts("DWORD"); break;
            case QWORD: puts("QWORD"); break;
            default: puts("\e[31mINVALID SIZE\e[0m"); break;
        }
    }
    else if(REGISTER == instruction.token_type){
        fputs("    REGISTER: ", stdout);

        if(0 <= instruction.data.reg.reg && instruction.data.reg.reg < NUM_REG_SIZE){
            printf("r%i\n", instruction.data.reg.reg);
        }
        else{
            printf("%s\n", regs[instruction.data.reg.reg - NUM_REG_SIZE][0]);
        }
    }
    else if(STRING == instruction.token_type){
        printf("    TAG: %s\n", instruction.data.str);
    }
    
    else{
        printf("    DATA: %li\n", instruction.data.num);
    }

cleanup:
    return;
}

void print_instructions(){
    int i=0;

    puts("---------------------");
    for(i=0; i<INSTRUCTION_SIZE && instructions[i].token_type != NONE; i++){
        print_instruction(instructions[i]);
    }

    puts("---------------------\n");
}