#include "inter.h"
#include "termios.h"

int errnum = 0;

int get_next_instruction(FILE * source, instruction_t * instruction, int line_no){
    int return_value = 0;
    
    return_value = rread(&instruction->token_type, sizeof(instruction->token_type), 1, source);
    if(0 == return_value){
        if(!feof(source)){
            return_value = print_error("\e[31mGET_NEXT_INSTRUCTION\e[0m: Fread error", -1);
            goto cleanup;
        }
        else{
            instruction->token_type = NONE;
            goto cleanup;
        }
    }

    switch(instruction->token_type){
        case TOKEN: return_value = rread(&instruction->data.token, sizeof(instruction->data.token), 1, source); break;
        case REGISTER: return_value = rread(&instruction->data.reg, sizeof(instruction->data.reg), 1, source); break;
        case NUM: return_value = rread(&instruction->data.num, sizeof(instruction->data.num), 1, source); break;
        case SIZE: return_value = rread(&instruction->data.size, sizeof(instruction->data.size), 1, source); break;
        case TAGGEE:
        case STRING: return_value = rread(&instruction->data.str, sizeof(instruction->data.str), 1, source); break;
        case FUNCTION: return_value = rread(&instruction->data.function, sizeof(instruction->data.function), 1, source); break;
        case FLAG: return_value = rread(&instruction->data.flag, sizeof(instruction->data.flag), 1, source); break;
        case NONE: break;
        default: {
            printf("\e[31mError\e[0m on line \e[31m%i\e[0m: Invalid token type value (%i) (index %i)\n", line_no, instruction->token_type, reg_struct.etp);
            return_value = -1;
            goto cleanup;
        }
    }

cleanup:
    return return_value;
}

int get_next_sequence(FILE * source, int * line_no){
    int return_value = 0;

    return_value = rread(line_no, sizeof(short int), 1, source);
    if(0 == return_value){
        if(!feof(source)){
            return_value = print_error("\e[31mGET_NEXT_INSTRUCTION\e[0m: Fread error", -1);
            goto cleanup;
        }
        else{
            return_value = 1;
            goto cleanup;
        }
    }

    for(reg_struct.etp=0; reg_struct.etp<sizeof(instructions)/sizeof(instructions[0]) && !feof(source); reg_struct.etp++){
        return_value = get_next_instruction(source, &(instructions[reg_struct.etp]), *line_no);
        if(-1 == return_value){
            goto cleanup;
        }

        if(NONE == instructions[reg_struct.etp].token_type){
            break;
        }
    }

    reg_struct.etp = 0;

cleanup:
    return return_value;
}

int * get_pointer_value(FILE * source, int line_no){
    char * return_value = 0;
    int error_check = 0;
    long int temp = 0;
    bool_t first = true;
    union reg_u * temp_register = NULL;

    while( !(TOKEN == instructions[reg_struct.etp].token_type && 
            OUT == instructions[reg_struct.etp].data.token) &&  
            NONE != instructions[reg_struct.etp].token_type){
            
        if(TOKEN == instructions[reg_struct.etp].token_type && IN == instructions[reg_struct.etp].data.token){
            reg_struct.etp ++;
            continue;
        }

        if(REGISTER == instructions[reg_struct.etp].token_type){
            temp = get_reg(&temp_register);
            if(-1 == temp){
                return_value = FAIL;
                errnum = 1;
                goto cleanup;
            }

            error_check = get_reg_value(temp_register, &temp);
            if(-1 == error_check){
                return_value = FAIL;
                errnum = 1;
                goto cleanup;
            }

            if(first || (TOKEN == instructions[reg_struct.etp-1].token_type && ADD == instructions[reg_struct.etp-1].data.token)){
                return_value += temp;
            }
            else if(TOKEN == instructions[reg_struct.etp-1].token_type && SUB == instructions[reg_struct.etp-1].data.token){
                return_value -= temp;
            }
            else{
                printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: unexpected token value\n", line_no);
                return_value = FAIL;
                errnum = 1;
                goto cleanup;
            }
        }
        
        else if(NUM == instructions[reg_struct.etp].token_type){
            if(first || (TOKEN == instructions[reg_struct.etp-1].token_type && ADD == instructions[reg_struct.etp-1].data.token)){
                return_value += instructions[reg_struct.etp].data.num;
            }
            else if(TOKEN == instructions[reg_struct.etp-1].token_type && SUB == instructions[reg_struct.etp-1].data.token){
                return_value -= instructions[reg_struct.etp].data.num;
            }
            else{
                printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: unexpected token value\n", line_no);
                return_value = FAIL;
                errnum = 1;
                goto cleanup;
            }
        }

        else if(STRING == instructions[reg_struct.etp].token_type){
            temp = (long int)instructions[reg_struct.etp].data.str + (long int)text;
            if(first || (TOKEN == instructions[reg_struct.etp-1].token_type && ADD == instructions[reg_struct.etp-1].data.token)){
                return_value += temp;
            }
            else if(TOKEN == instructions[reg_struct.etp-1].token_type && SUB == instructions[reg_struct.etp-1].data.token){
                return_value -= temp;
            }
            else{
                printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: unexpected token value\n", line_no);
                return_value = FAIL;
                errnum = 1;
                goto cleanup;
            }
        }

        else if(TAGGEE == instructions[reg_struct.etp].token_type){
            temp = (long int)instructions[reg_struct.etp].data.str;
            if(first || (TOKEN == instructions[reg_struct.etp-1].token_type && ADD == instructions[reg_struct.etp-1].data.token)){
                return_value += temp;
            }
            else if(TOKEN == instructions[reg_struct.etp-1].token_type && SUB == instructions[reg_struct.etp-1].data.token){
                return_value -= temp;
            }
            else{
                printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: unexpected token value\n", line_no);
                return_value = FAIL;
                errnum = 1;
                goto cleanup;
            }
        }

        reg_struct.etp ++;
        first = false;
    }

    if(TOKEN == instructions[reg_struct.etp].token_type && OUT == instructions[reg_struct.etp].data.token){
        reg_struct.etp ++;
    }

    //printf("VALUE: %p %p\n", return_value, (void *)(reg_struct.rsp-8));

cleanup:
    return (int *)return_value;
}

int execute_instructions(FILE * source, int line_no){
    int return_value = 0;
    int size = NONE;
    long int temp = 0;
    long int temp1 = 0;
    int i = 0;
    int * r1 = NULL;
    int * r2 = NULL;
    union reg_u * r3 = NULL;
    union reg_u * r4 = NULL;
    union reg_u * temp_register = NULL;

    if(TOKEN != instructions[0].token_type){
        printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: expected token\n", line_no);
        return_value = -1;
        goto cleanup;
    }
    if(END == instructions[0].data.token){
        return_value = -1;
        goto cleanup;
    }

    reg_struct.etp = 0;

    switch(instructions[reg_struct.etp].data.token){
        case PUSH: {
            reg_struct.etp ++;
            if(NUM == instructions[reg_struct.etp].token_type){
                *((long int *)reg_struct.rsp.reg_64) = instructions[1].data.num;
            }
            else if(REGISTER == instructions[reg_struct.etp].token_type){
                return_value = get_reg(&temp_register);
                if(-1 == return_value){
                    goto cleanup;
                }

                return_value = get_reg_value(temp_register, &temp);
                if(-1 == return_value){
                    goto cleanup;
                }

                *(long int *)reg_struct.rsp.reg_64 = temp;
            }
            else{
                printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: expected register or integral type\n", line_no);
                return_value = -1;
                goto cleanup;
            }

            reg_struct.rsp.reg_64 += STACK_ELEMENT_SIZE;
            break;
        }

        case POP: {
            reg_struct.etp ++;
            if(REGISTER == instructions[reg_struct.etp].token_type){
                reg_struct.rsp.reg_64 -= STACK_ELEMENT_SIZE;

                return_value = get_reg(&temp_register);
                if(-1 == return_value){
                    goto cleanup;
                }

                i = instructions[reg_struct.etp].data.reg.index;

                switch(instructions[reg_struct.etp].data.reg.size){
                    case R_REG_SIZE: temp_register->reg_64 = *(long int *)reg_struct.rsp.reg_64; break;
                    case E_REG_SIZE: temp_register->reg_32[i] = *(long int *)reg_struct.rsp.reg_64; break;
                    case X_REG_SIZE: temp_register->reg_16[i] = *(long int *)reg_struct.rsp.reg_64; break;
                    case HL_REG_SIZE: temp_register->reg_8[i] = *(long int *)reg_struct.rsp.reg_64; break;
                    default:
                        printf("\e[31mError\e[0m invalid register size on line \e[31m%i\e[0m", line_no);
                        return_value = -1;
                        goto cleanup;
                }
            }
            else if(NONE == instructions[reg_struct.etp].token_type){
                reg_struct.rsp.reg_64 -= STACK_ELEMENT_SIZE;
            }
            else{
                printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: expected register type\n", line_no);
                return_value = -1;
                goto cleanup;
            }

            

            break;
        }

        case MOV: {
            reg_struct.etp ++;
            if(SIZE == instructions[reg_struct.etp].token_type){
                size = instructions[reg_struct.etp].data.size;

                reg_struct.etp ++;
            }

            if(TOKEN == instructions[reg_struct.etp].token_type && IN == instructions[reg_struct.etp].data.token){        //MOV [p1] ____
                if(0 == size){
                    printf("\e[31mError\e[0m on line \e[31m%i\e[0m: Expected size token\n", line_no);
                    return_value = -1;
                    goto cleanup;
                }

                //Get p1 
                reg_struct.etp ++;
                r1 = get_pointer_value(source, line_no);
                if(FAIL == r1 && errnum == 1){
                    puts("EXECUTE_INSTRUCTIONS: Get_pointer_value error");
                    return_value = -1;
                    goto cleanup;
                }

                //printf("%i TOKEN VALUE: %i\n", reg_struct.etp, instructions[reg_struct.etp].token_type);

                if(TOKEN == instructions[reg_struct.etp].token_type && IN == instructions[reg_struct.etp].data.token){    //MOV [p1] [p2] (eg. MOV [rax] [rbp - 16])
                    r2 = get_pointer_value(source, line_no);
                    if(FAIL == r2 && 1 == errnum){
                        puts("EXECUTE_INSTRUCTIONS: Get_pointer_value error");
                        return_value = -1;
                        goto cleanup;
                    }
                    
                    switch(size){
                        case BYTE: *(char *)r1 = *(char *)r2; break;
                        case WORD: *(short int *)r1 = *(short int *)r2; break;
                        case DWORD: *(int *)r1 = *(int *)r2; break;
                        case QWORD: *(long int *)r1 = *(long int *)r2; break;
                        default: printf("\e[31mError\e[0m on line %i: Invalid size value", line_no); return_value = -1; goto cleanup;
                    }
                }
                else if(REGISTER == instructions[reg_struct.etp].token_type){      //MOV [p1] reg (eg. MOV [rax] rcx)
                    return_value = get_reg(&temp_register);
                    if(-1 == return_value){
                        goto cleanup;
                    }

                    return_value = get_reg_value(temp_register, &temp);
                    if(-1 == return_value){
                        goto cleanup;
                    }

                    switch(size){
                        case BYTE: *(char *)r1 = (char)temp; break;
                        case WORD: *(short int *)r1 = (short int)temp; break;
                        case DWORD: *(int *)r1 = (int)temp; break;
                        case QWORD: *(long int *)r1 = (long int)temp; break;
                        default: printf("\e[31mError\e[0m on line %i: Invalid size value", line_no); return_value = -1; goto cleanup;
                    }
                }
                else if(NUM == instructions[reg_struct.etp].token_type){        //MOV [p1] NUM [eg. MOV [rax] 121]
                    temp = instructions[reg_struct.etp].data.num;

                    switch(size){
                        case BYTE: *(char *)r1 = (char)temp; break;
                        case WORD: *(short int *)r1 = (short int)temp; break;
                        case DWORD: *(int *)r1 = (int)temp; break;
                        case QWORD: *(long int *)r1 = (long int)temp; break;
                        default: printf("\e[31mError\e[0m on line %i: Invalid size value", line_no); return_value = -1; goto cleanup;
                    }
                }
            }
            else if(REGISTER == instructions[reg_struct.etp].token_type){        //MOV reg ____
                i = reg_struct.etp;
                return_value = get_reg(&r3);
                if(-1 == return_value){
                    goto cleanup;
                }

                reg_struct.etp ++;

                if(TOKEN == instructions[reg_struct.etp].token_type && IN == instructions[reg_struct.etp].data.token){        //MOV reg [p1] (eg. MOV rax [rbp + 16])
                    if(0 == size){
                        printf("\e[31mError\e[0m on line \e[31m%i\e[0m: Expected size token\n", line_no);
                        return_value = -1;
                        goto cleanup;
                    }

                    reg_struct.etp ++;
                    r1 = get_pointer_value(source, line_no);
                    if(FAIL == r1 && 1 == errnum){
                        puts("EXECUTE_INSTRUCTIONS: Get_pointer_value error");
                        return_value = -1;
                        goto cleanup;
                    }

                    switch(size){
                        case BYTE: temp = *(char *)r1; break;
                        case WORD: temp = *(short int *)r1; break;
                        case DWORD: temp = *(int *)r1; break;
                        case QWORD: temp = *(long int *)r1; break;
                    }
                    return_value = set_reg_value(r3, temp, i);
                    if(-1 == return_value){
                        goto cleanup;
                    }
                }
                else if(REGISTER == instructions[reg_struct.etp].token_type){    //MOV reg1 reg2 (eg. MOV rax rcx)
                    return_value = get_reg(&r4);
                    if(-1 == return_value){
                        goto cleanup;
                    }

                    return_value = get_reg_value(r4, &temp);
                    if(-1 == return_value){
                        goto cleanup;
                    }

                    return_value = set_reg_value(r3, temp, i);
                    if(-1 == return_value){
                        goto cleanup;
                    }
                }
                else if(NUM == instructions[reg_struct.etp].token_type){     //MOV reg1 NUM (eg. MOV rax 121)
                    temp = instructions[reg_struct.etp].data.num;

                    return_value = set_reg_value(r3, temp, i);
                }
            }

            break;
        }
    
        case LEA: {
            reg_struct.etp ++;
            if(REGISTER == instructions[reg_struct.etp].token_type){
                return_value = get_reg(&r3);
                if(-1 == return_value){
                    goto cleanup;
                }

                i = reg_struct.etp;
                reg_struct.etp ++;

                if(TOKEN != instructions[reg_struct.etp].token_type || IN != instructions[reg_struct.etp].data.token){
                    printf("\e[31mError\e[0m on line \e[31m%i\e[0m: LEA must be followed by a [ token\n", line_no);
                    return_value = -1;
                    goto cleanup;
                }

                temp = (long int)get_pointer_value(source, line_no);
                if(-1 == temp && 1 == errnum){
                    return_value = -1;
                    goto cleanup;
                }

                return_value = set_reg_value(r3, temp, i);
                if(-1 == return_value){
                    goto cleanup;
                }
            }
            else{
                printf("\e[31mError\e[0m on line \e[31m%i\e[0m: expected register as first parameter\n", line_no);
                return_value = -1;
                goto cleanup;
            }

            break;
        }

        case JMP: {
            reg_struct.etp ++;
            return_value = jump(source, line_no);
            if(-1 == return_value){
                goto cleanup;
            }

            break;
        }

        case CALL: {
            reg_struct.etp ++;
            
            if(FUNCTION == instructions[reg_struct.etp].token_type){
                switch(instructions[reg_struct.etp].data.function){
                    case OPEN: {
                        temp = 0;

                        for(i=0; i<reg_struct.rbx.reg_64; i++){
                            reg_struct.rsp.reg_64 -= STACK_ELEMENT_SIZE;
                            temp |= *(long int *)reg_struct.rsp.reg_64;
                        }

                        reg_struct.rax.reg_32[0] = open((const char *)reg_struct.rax.reg_64, temp);

                        break;
                    }
                    case READ: {
                        reg_struct.rax.reg_64 = read(reg_struct.rax.reg_32[0], (void *)reg_struct.rbx.reg_64, reg_struct.rcx.reg_64);
                        break;
                    }
                    case WRITE: {
                        reg_struct.rax.reg_64 = write(reg_struct.rax.reg_32[0], (void *)reg_struct.rbx.reg_64, reg_struct.rcx.reg_64);
                        break;
                    }
                    case PRNUM: {
                        printf("%li\n", reg_struct.rax.reg_64);
                        break;
                    }
                    default: {
                        printf("\e[31mError\e[0m on line \e[31m%i\e[0m: Invalid function value\n", line_no);
                        return_value = -1;
                        goto cleanup;
                    }
                }
            }
            else if(TAGGEE == instructions[reg_struct.etp].token_type){
                temp = (long int)instructions[reg_struct.etp].data.str + code_start;

                *(long int *)reg_struct.rsp.reg_64 = reg_struct.rip.reg_64;
                reg_struct.rsp.reg_64 += STACK_ELEMENT_SIZE;

                return_value = rseek(source, temp, SEEK_SET);
                if(-1 == return_value){
                    return_value = print_error("\e[31mEXECUTE_INSTRUCTIONS\e[0m: Fseek error", -1);
                    goto cleanup;
                }
            }
            else{
                printf("\e[31mError\e[0m on line \e[31m%i\e[0m: CALL instruction expects tag or function\n", line_no);
                return_value = -1;
                goto cleanup;
            }

            break;
        } 

        case CMP: {
            reg_struct.etp ++;

            if(REGISTER == instructions[reg_struct.etp].token_type){
                return_value = get_reg(&r3);
                if(-1 == return_value){
                    goto cleanup;
                }

                return_value = get_reg_value(r3, &temp);
                if(-1 == return_value){
                    goto cleanup;
                }
            }
            else if(NUM == instructions[reg_struct.etp].token_type){
                temp = instructions[reg_struct.etp].data.num;
            }
            else{
                printf("\e[31mError\e[0m on line \e[31m%i\e[0m: expected register or integral type\n", line_no);
                return_value = -1;
                goto cleanup;
            }

            reg_struct.etp ++;

            if(REGISTER == instructions[reg_struct.etp].token_type){
                return_value = get_reg(&r3);
                if(-1 == return_value){
                    goto cleanup;
                }

                return_value = get_reg_value(r3, &temp1);
                if(-1 == return_value){
                    goto cleanup;
                }
            }
            else if(NUM == instructions[reg_struct.etp].token_type){
                temp1 = instructions[reg_struct.etp].data.num;
            }
            else{
                printf("\e[31mError\e[0m on line \e[31m%i\e[0m: expected register or integral type\n", line_no);
                return_value = -1;
                goto cleanup;
            }

            temp -= temp1;

            if(0 == temp){
                flags.zf = 1;
                flags.sf = 0;
            }
            else if(temp > 0){
                flags.zf = 0;
                flags.sf = 0;
            }
            else{
                flags.zf = 0;
                flags.sf = 1;
            }

            break;
        }
        
        case JE: {
            reg_struct.etp ++;
            if(flags.zf){
                return_value = jump(source, line_no);
                if(-1 == return_value){
                    goto cleanup;
                }
            }

            break;
        }

        case JNE: {
            reg_struct.etp ++;
            if(!flags.zf){
                return_value = jump(source, line_no);
                if(-1 == return_value){
                    goto cleanup;
                }
            }

            break;
        }

        case JLE: {
            reg_struct.etp ++;
            if(flags.zf || flags.sf){
                return_value = jump(source, line_no);
                if(-1 == return_value){
                    goto cleanup;
                }
            }

            break;
        }

        case JGE: {
            reg_struct.etp ++;
            if(flags.zf || !flags.sf){
                return_value = jump(source, line_no);
                if(-1 == return_value){
                    goto cleanup;
                }
            }

            break;
        }

        case JL: {
            reg_struct.etp ++;
            if(!flags.zf && flags.sf){
                return_value = jump(source, line_no);
                if(-1 == return_value){
                    goto cleanup;
                }
            }

            break;
        }

        case JG: {
            reg_struct.etp ++;
            if(!flags.zf && !flags.sf){
                return_value = jump(source, line_no);
                if(-1 == return_value){
                    goto cleanup;
                }
            }

            break;
        }
        
        case DIV:
        case MUL:
        case SUB:
        case ADD: {
            i = reg_struct.etp;

            reg_struct.etp ++;
            if(REGISTER != instructions[reg_struct.etp].token_type){
                printf("\e[31mError\e[0m on line \e[31m%i\e[0m: Expected register value\n", line_no);
                return_value = -1;
                goto cleanup;
            }

            return_value = get_reg(&r3);
            if(-1 == return_value){
                goto cleanup;
            }

            return_value = get_reg_value(r3, &temp);
            if(-1 == return_value){
                goto cleanup;
            }

            reg_struct.etp ++;
            if(REGISTER == instructions[reg_struct.etp].token_type){
                return_value = get_reg(&r4);
                if(-1 == return_value){
                    goto cleanup;
                }

                return_value = get_reg_value(r4, &temp1);
                if(-1 == return_value){
                    goto cleanup;
                }
            }
            else if(NUM == instructions[reg_struct.etp].token_type){
                temp1 = instructions[reg_struct.etp].data.num;
            }

            switch(instructions[i].data.token){
                case ADD: temp += temp1; break;
                case SUB: temp -= temp1; break;
                case MUL: temp *= temp1; break;
                case DIV: temp /= temp1; break;
                default: break;
            }

            return_value = set_reg_value(r3, temp, i+1);
            if(-1 == return_value){
                goto cleanup;
            }
            
            break;
        }

        default: printf("\e[33mWarning\e[0m on line \e[33m%i\e[0m: unsupported token. (%i) (These may be added in later versions)\n", line_no, instructions[0].data.token); goto cleanup;
    }

    reg_struct.etp ++;

    for(; reg_struct.etp < INSTRUCTION_SIZE; reg_struct.etp++){
        if(instructions[reg_struct.etp].token_type != NONE){
            printf("\e[33mWarning\e[0m on line \e[33m%i\e[0m: excess tokens (index: \e[33m%i\e[0m)\n(Excess tokens are ignored)\n", line_no, reg_struct.etp);
            printf("TOKEN TYPE: %i TOKEN VALUE: %li\n", instructions[reg_struct.etp].token_type, instructions[reg_struct.etp].data.num);
            //break;
        }
    }

cleanup:
    return return_value;
}

bool_t read_header(FILE * source, off_t * entry_offset){
    bool_t is_valid = true;
    int error_check = 0;
    int version_check[3] = {0};
    char * magic_check = NULL;

    magic_check = calloc(magic_len + 1, sizeof(char));
    if(NULL == magic_check){
        is_valid = print_error("\e[31mREAD_HEADER\e[0m: Calloc error", false);
        goto cleanup;
    }

    error_check = fread(magic_check, sizeof(char), magic_len+1, source);
    if(0 == error_check){
        if(feof(source)){
            puts("\e[31mError\e[0m: Invalid file");
            is_valid = false;
            goto cleanup;
        }
        else{
            is_valid = print_error("\e[31mREAD_HEADER\e[0m: Fread error", false);
            goto cleanup;
        }
    }

    error_check = strncmp(magic, magic_check, magic_len+1);
    if(error_check != 0){
        puts("\e[31mError\e[0m: Invalid file");
        is_valid = false;
        goto cleanup;
    }

    error_check = fread(version_check, sizeof(version_check[0]), sizeof(version_check)/sizeof(version_check[0]), source);
    if(0 == error_check){
        if(feof(source)){
            puts("\e[31mError\e[0m: Invalid file");
            is_valid = false;
            goto cleanup;
        }
        else{
            is_valid = print_error("\e[31mREAD_HEADER\e[0m: Fread error", false);
            goto cleanup;
        }
    }

    if(version_check[0] < oldest_compatible[0]){
        puts("\e[31mError\e[0m: Compiled file is incompatible with current version.");
        is_valid = false;
        goto cleanup;
    }
    else if(version_check[0] == oldest_compatible[0]){
        if(version_check[1] < oldest_compatible[1]){
            puts("\e[31mError\e[0m: Compiled file is incompatible with current version.");
            is_valid = false;
            goto cleanup;
        }
        else if(version_check[1] == oldest_compatible[1]){
            if(version_check[2] < oldest_compatible[2]){
                puts("\e[31mError\e[0m: Compiled file is incompatible with current version.");
                is_valid = false;
                goto cleanup;
            }
        }
    }

    error_check = fread(entry_offset, sizeof(*entry_offset), 1, source);
    if(0 == error_check){
        if(feof(source)){
            puts("\e[31mError\e[0m: Invalid file");
            is_valid = false;
            goto cleanup;
        }
        else{
            is_valid = print_error("\e[31mREAD_HEADER\e[0m: Fread error", false);
            goto cleanup;
        }
    }

cleanup:
    if(NULL != magic_check){
        free(magic_check);
    }

    return is_valid;
}

int get_text(FILE * source, long int * map_size){
    long int text_length = 0;
    int error_check = 0;
    int page_size = 0;

    error_check = fread(&text_length, sizeof(text_length), 1, source);
    if(0 == error_check){
        if(feof(source)){
            puts("\e[31mError\e[0m: Invalid file");
            error_check = -1;
            goto cleanup;
        }
        else{
            error_check = print_error("\e[31mGET_TEXT\e[0m: Fread error", -1);
            goto cleanup;
        }
    }

    page_size = getpagesize();

    if(text_length > 0){
        if(text_length % page_size == 0){
            *map_size = text_length;
        }
        else{
            *map_size = (text_length / page_size + 1) * page_size;
        }

        text = mmap(NULL, *map_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        if(MAP_FAILED == text){
            error_check = print_error("\e[31mGET_TEXT\e[0m: Mmap error", -1);
            goto cleanup;
        }

        reg_struct.rtp.reg_64 = (long int)text + text_length;

        error_check = fread(text, sizeof(char), text_length, source);
        if(0 == error_check && !feof(source) && text_length != 0){
            error_check = print_error("\e[31mGET_TEXT\e[0m: Fread error", -1);
            goto cleanup;
        }
    }

    error_check = 0;

cleanup:
    return error_check;
}

int execute(char * filename, func_flags_t fun_flags){
    int return_value = 0;
    int line_no = 0;
    long int map_size = 0;
    off_t entry_off = 0;
    off_t length = 0;
    bool_t is_valid = false;
    FILE * source = NULL;

    source = fopen(filename, "r");
    if(NULL == source){
        return_value = print_error("\e[31mEXECUTE: Fopen error\e[0m", -1);
        goto cleanup;
    }

    is_valid = read_header(source, &entry_off);
    if(!is_valid){
        goto cleanup;
    }

    return_value = get_text(source, &map_size);
    if(-1 == return_value){
        goto cleanup;
    }

    code_start = ftell(source);

    return_value = fseek(source, 0, SEEK_END);
    if(-1 == return_value){
        return_value = print_error("\e[31mEXECUTE: Fseek error\e[0m", -1);
        goto cleanup;
    }

    length = ftell(source);

    return_value = rseek(source, entry_off + code_start, SEEK_SET);
    if(-1 == return_value){
        return_value = print_error("\e[31mEXECUTE: Fseek error\e[0m", -1);
        goto cleanup;
    }

    while(reg_struct.rip.reg_64 <= length){
        return_value = get_next_sequence(source, &line_no);
        if(-1 == return_value){
            goto cleanup;
        }

        if(fun_flags.print_instructions){
            print_instructions();
        }
        return_value = execute_instructions(source, line_no);
        if(-1 ==return_value){
            goto cleanup;
        }

        if(fun_flags.print_regs){
            print_regs();
        }
        if(fun_flags.print_stack){
            print_stack();
        }
        if(fun_flags.print_flags){
            print_flags();
        }

        memset(instructions, 0, sizeof(instruction_t) * INSTRUCTION_SIZE);
        reg_struct.etp = 0;

        if(feof(source) && reg_struct.rip.reg_64 <= length){
            clearerr(source);
        }
    }

    munmap(stack, getpagesize());
    munmap(text, map_size);

cleanup:
    free_jump_offsets();
    free_vars();
    if(NULL != source){
        fclose(source);
    }

    return return_value;
}

int main(int argc, char ** argv){
    int error_check = 0;
    int i = 0;
    int j = 0;
    int file_index = 0;
    func_flags_t fun_flags = {0};

    for(i=1; i<argc; i++){
        if('-' == argv[i][0]){
            for(j=1; argv[i][j] != 0; j++){
                switch(argv[i][j]){
                    case 'r': fun_flags.print_regs = 1; break;
                    case 's': fun_flags.print_stack = 1; break;
                    case 'i': fun_flags.print_instructions = 1; break;
                    case 'f': fun_flags.print_flags = 1; break;
                    case 'h': print_help(); goto cleanup;
                    default:
                        printf("\e[31mUsage\e[0m: \e[31m%s\e[0m: Invalid flag \e[31m%c\e[0m\nExiting...\n", argv[0], argv[i][j]);
                        goto cleanup;
                }
            }
        }
        else{
            file_index = i;
        }
    }

    error_check = init();
    if(-1 == error_check){
        goto cleanup;
    }

    error_check = execute(argv[file_index], fun_flags);

cleanup:
    tcflush(STDIN_FILENO, TCIFLUSH);
    
    exit(error_check);
}
