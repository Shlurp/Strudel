#include "inter.h"

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

void print_regs(){
    int i = 0;

    printf("\e[1mRIP\e[0m: %li | \e[1mRSP\e[0m: %li | \e[1mRTP\e[0m: %li | \e[1mRBP\e[0m: %li | \e[1mRAX\e[0m: %li | \e[1mRBX\e[0m: %li| \e[1mRCX\e[0m: %li| \e[1mRDX\e[0m: %li\n", reg_struct.rip.reg_64, reg_struct.rsp.reg_64, reg_struct.rtp.reg_64, reg_struct.rbp.reg_64, reg_struct.rax.reg_64, reg_struct.rbx.reg_64, reg_struct.rcx.reg_64, reg_struct.rdx.reg_64);

    for(i=0; i<NUM_REG_SIZE; i++){
        printf("\e[1mr%i\e[0m: %li | ", i, reg_struct.rx[i].reg_64);
    }

    putchar('\n');
}

void print_stack(){
    long int i = 0;
    int j = 0;

    for(i=(long int)stack; i<=reg_struct.rsp.reg_64; i+=STACK_ELEMENT_SIZE){
        printf("%p: %li", (void *)i, *(long int *)i);

        if(i <= reg_struct.rsp.reg_64 && reg_struct.rsp.reg_64 < i + STACK_ELEMENT_SIZE){
            fputs("\e[32m < RSP\e[0m", stdout);
        }
        if(i <= reg_struct.rbp.reg_64 && reg_struct.rbp.reg_64 < i + STACK_ELEMENT_SIZE){
            fputs("\e[32m < RBP\e[0m", stdout);
        }
        if(i <= reg_struct.rax.reg_64 && reg_struct.rax.reg_64 < i + STACK_ELEMENT_SIZE){
            fputs("\e[32m < RAX\e[0m", stdout);
        }
        if(i <= reg_struct.rbx.reg_64 && reg_struct.rbx.reg_64 < i + STACK_ELEMENT_SIZE){
            fputs("\e[32m < RBX\e[0m", stdout);
        }
        if(i <= reg_struct.rcx.reg_64 && reg_struct.rcx.reg_64 < i + STACK_ELEMENT_SIZE){
            fputs("\e[32m < RCX\e[0m", stdout);
        }
        if(i <= reg_struct.rdx.reg_64 && reg_struct.rdx.reg_64 < i + STACK_ELEMENT_SIZE){
            fputs("\e[32m < RDX\e[0m", stdout);
        }

        for(j=0; j<NUM_REG_SIZE; j++){
            if(i <= reg_struct.rx[j].reg_64 && reg_struct.rx[j].reg_64 < i + STACK_ELEMENT_SIZE){
                printf("\e[32m < r%i\e[0m", j);
            }
        }

        putchar('\n');
    }

    putchar('\n');
}

void print_flags(){
    printf("\e[1mZERO\e[0m: %i | \e[1mSIGNED\e[0m: %i\n", flags.zf, flags.sf);
}

int get_curr_line(FILE * source){
    off_t origin_offset = 0;
    off_t curr_offset = 0;
    int error_check = 0;
    int num_of_lines = 0;
    char curr_char = 0;

    origin_offset = ftell(source);
    if(-1 == origin_offset){
        num_of_lines = print_error("\e[31mGET_CURR_LINE\e[0m: Ftell error", -1);
        goto cleanup;
    }

    curr_offset = fseek(source, curr_offset, SEEK_SET);
    if(-1 == curr_offset){
        num_of_lines = print_error("\e[31mGET_CURR_LINE\e[0m: Fseek error", -1);
        goto cleanup;
    }

    do{
        do{
            error_check = fread(&curr_char, sizeof(curr_char), 1, source);
            if(-1 == error_check){
                num_of_lines = print_error("\e[31mGET_CURR_LINE\e[0m: Fread error", -1);
                goto cleanup;
            }
        }while(curr_char != '\n' && !feof(source));

        num_of_lines ++;

        curr_offset = ftell(source);
        if(-1 == curr_offset){
            num_of_lines = print_error("\e[31mGET_CURR_LINE\e[0m: Ftell error", -1);
            goto cleanup;
        }
    }while(curr_offset < origin_offset);

    curr_offset = fseek(source, origin_offset, SEEK_SET);
    if(-1 == curr_offset){
        num_of_lines = print_error("\e[31mGET_CURR_LINE\e[0m: Fseek error", -1);
        goto cleanup;
    }

cleanup:
    return num_of_lines;
}
