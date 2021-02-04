#include "inter.h"

char * tokens[] = {"PUSH", "POP", "MOV", "LEA", "CMP", "JMP", "CALL", "JE", "JNE", "JG", "JGE", "JL", "JLE", "ADD", "SUB", "MUL", "DIV", "TAG", "SET", "[", "]", "END"};
char * sizes[] = {"BYTE", "WORD", "DWORD", "QWORD"};
char * regs[][5] = {{"RIP"},
                    {"RSP"}, 
                    {"RTP"},
                    {"RBP"}, 
                    {"RAX", "EAX", "AX", "AH", "AL"}, 
                    {"RBX", "EBX", "BX", "BH", "BL"}, 
                    {"RCX", "ECX", "CX", "CH", "CL"}, 
                    {"RDX", "EDX", "DX", "DH", "DL"}};

int get_token_str(FILE * source, char ** token_str){
    int return_value = 0;
    int buffer_pointer = 0;
    char curr_char = 0;
    bool_t first = true;
    bool_t str = false;
    char buffer[BUFFER_SIZE] = {0};

    while(true){
        curr_char = rgetc(source);
        if(EOF == curr_char && !feof(source)){
            return_value = print_error("\e[31mGET_TOKEN_STR: Fread error\e[0m", -1);
            goto cleanup;
        }
        else if((feof(source)) || ((' ' == curr_char || ',' == curr_char) && !first && !str)){
            break;
        }
        else if('\n' == curr_char && !first){
            newline = true;
            break;
        }
        else if('"' == curr_char){
            str = ~str;
        }
        if(!str){
            if(';' == curr_char){
                while('\n' != curr_char && !feof(source)){
                    curr_char = rgetc(source);
                    if(EOF == curr_char && !feof(source)){
                        return_value = print_error("\e[31mGET_TOKEN_STR: Fread error\e[0m", -1);
                        goto cleanup;
                    }
                }
                
                if(0 == reg_struct.etp){
                    continue;
                }
                else{
                    newline = true;
                    break;
                }
            }
            else if((' ' == curr_char || ',' == curr_char) && first){
                continue;
            }
        }
        if('\n' == curr_char && first){
            continue;
        }

        if(']' == curr_char && !first && !str){
            return_value = rseek(source, -1, SEEK_CUR);
            if(-1 == return_value){
                print_error("\e[31mGET_TOKEN_STR: lseek error\e[0m", 0);
                goto cleanup;
            }
            break;
        } 

        first = false;

        buffer[buffer_pointer] = curr_char;

        if(('[' == curr_char || ']' == curr_char) && !str){
            curr_char = rgetc(source);
            if(EOF == curr_char && !feof(source)){
                return_value = print_error("\e[31mGET_TOKEN_STR: Fread error\e[0m", -1);
                goto cleanup;
            }
            else if(feof(source)){
                break;
            }

            if('\n' == curr_char){
                newline = true;
            }
            else{
                return_value = rseek(source, -1, SEEK_CUR);
                if(-1 == return_value){
                    print_error("\e[31mGET_TOKEN_STR: lseek error\e[0m", 0);
                    goto cleanup;
                }
            }
            break;
        }

        buffer_pointer ++;
    }

    return_value = strnlen(buffer, BUFFER_SIZE);
    *token_str = malloc(return_value+1);
    if(NULL == *token_str){
        return_value = print_error("\e[31mGET_TOKEN_STR: Malloc error\e[0m", -1);
        goto cleanup;
    }
    memset(*token_str, 0, return_value+1);

    memcpy(*token_str, buffer, return_value);

cleanup:
    return return_value;
}

int get_next_instruction(FILE * source, instruction_t * instruction){
    int return_value = 0;
    int token_length = 0;
    int difference = 0;
    int i = 0;
    int j = 0;
    char * token_str = NULL;

    memset(instruction, 0, sizeof(instruction_t));

    token_length = get_token_str(source, &token_str);
    if(-1 == token_length){
        puts("GET_NEXT_INSTRUCTION: Get_token_str error");
        return_value = -1;
        goto cleanup;
    }

    if(token_length == 1){
        if(token_str[0] == '['){
            instruction->token_type = TOKEN;
            instruction->data.token = IN;
            goto cleanup;
        }
        else if(token_str[0] == ']'){
            instruction->token_type = TOKEN;
            instruction->data.token = OUT;
            goto cleanup;
        }
        else if(token_str[0] == '+'){
            instruction->token_type = TOKEN;
            instruction->data.token = ADD;
            goto cleanup;
        }
        else if(token_str[0] == '-'){
            instruction->token_type = TOKEN;
            instruction->data.token = SUB;
            goto cleanup;
        }
    }
    else if(2 == token_length){
        if('r' == token_str[0]){
            instruction->token_type = REGISTER;
            instruction->data.reg.reg = token_str[1] - '0';
            instruction->data.reg.size = R_REG_SIZE;
            goto cleanup;
        }
    }

    for(i=0; i<sizeof(tokens)/sizeof(tokens[0]); i++){
        difference = strncmp(token_str, tokens[i], BUFFER_SIZE);
        if(0 == difference){
            instruction->token_type = TOKEN;
            instruction->data.token = i+1;

            /*if((JMP <= i+1 && i+1 <= JLE) || (TAG == i+1)){
                 = true;
            }*/

            goto cleanup;
        }
    }

    for(i=0; i<sizeof(sizes)/sizeof(sizes[0]); i++){
        difference = strncmp(token_str, sizes[i], BUFFER_SIZE);
        if(0 == difference){
            instruction->token_type = SIZE;
            instruction->data.size = i+1;
            goto cleanup;
        }
    }

    for(i=0; i<sizeof(regs)/sizeof(regs[0]); i++){
        for(j=0; j<sizeof(regs[i])/sizeof(regs[i][0]); j++){
            if(NULL == regs[i][j]){
                break;
            }

            difference = strncmp(token_str, regs[i][j], BUFFER_SIZE);
            if(0 == difference){
                instruction->token_type = REGISTER;
                instruction->data.reg.reg = NUM_REG_SIZE + i;
                
                /**
                 * This is the structure of a register for this program. In reality, it is flipped.
                 *                    R*X
                 *   _______________________________________
                 *  /       E*X                             \
                 *  | __________________                    |
                 *  |/   *X             \                   |
                 *  || _______          |                   |
                 *  ||/       \         |                   |
                 *  +----+----+----+----+----+----+----+----+
                 *  | *L | *H |    |    |    |    |    |    |
                 *  |    |    |    |    |    |    |    |    |
                 *  +----+----+----+----+----+----+----+----+
                 */

                if('R' == token_str[0]){
                    instruction->data.reg.size = R_REG_SIZE;
                }
                else if('E' == token_str[0]){
                    instruction->data.reg.size = E_REG_SIZE;
                    instruction->data.reg.index = 0;
                }
                else if('X' == token_str[1]){
                    instruction->data.reg.size = X_REG_SIZE;
                    instruction->data.reg.index = 0;
                }
                else if('H' == token_str[1]){
                    instruction->data.reg.size = HL_REG_SIZE;
                    instruction->data.reg.index = 1;
                }
                else if('L' == token_str[1]){
                    instruction->data.reg.size = HL_REG_SIZE;
                    instruction->data.reg.index = 0;
                }
                else{
                    printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: token: \e[31m%s\e[0m matches with register \e[31m%s\e[0m, but is not a valid register\nExiting...\n", get_curr_line(source), token_str, regs[i][j]);
                    return_value = -1;
                    goto cleanup;
                }
                goto cleanup;
            }
        }
    }

    j = 1;
    if('-' == token_str[0]){
        i = 1;
    }
    else{
        i = 0;
    }
    for(; i<token_length; i++){
        if(token_str[i] < '0' || token_str[i] > '9'){
            j = 0;
            break;
        }

        instruction->token_type = NUM;
        instruction->data.num = instruction->data.num * 10 + token_str[i] - '0';
    }

    if(1 == j){
        if('-' == token_str[0]){
            instruction->data.num *= -1;
        }
    }
    else{
        instruction->token_type = STRING;
        instruction->data.str = calloc(token_length+1, sizeof(char));
        if(NULL == instruction->data.str){
            return_value = print_error("\e[31mGET_NEXT_INSTRUCTION\e[0m: calloc error", -1);
            goto cleanup;
        }
        memcpy(instruction->data.str, token_str, token_length);
    }

cleanup:
    if(NULL != token_str){
        free(token_str);
    }
    return return_value;
}

int get_next_sequence(FILE * source){
    int return_value = 0;

    for(reg_struct.etp=0; reg_struct.etp<sizeof(instructions)/sizeof(instructions[0]) && !newline && !feof(source); reg_struct.etp++){
        return_value = get_next_instruction(source, &(instructions[reg_struct.etp]));
        if(-1 == return_value){
            goto cleanup;
        }
    }
    newline = false;

    reg_struct.etp = 0;

cleanup:
    return return_value;
}

int * get_pointer_value(FILE * source){
    char * return_value = 0;
    int error_check = 0;
    long int temp = 0;
    bool_t first = true;
    union reg_u * temp_register = NULL;

    while( !(TOKEN == instructions[reg_struct.etp].token_type && 
            OUT == instructions[reg_struct.etp].data.token) &&  
            NONE != instructions[reg_struct.etp].token_type){

        //printf("TOKEN TYPE: %i\nVALUE: %i\n", instructions[reg_struct.etp].token_type, instructions[reg_struct.etp].data.token);
            
        if(TOKEN == instructions[reg_struct.etp].token_type && IN == instructions[reg_struct.etp].data.token){
            reg_struct.etp ++;
            continue;
        }

        if(REGISTER == instructions[reg_struct.etp].token_type){
            temp = get_reg(&temp_register);
            if(-1 == temp){
                return_value = FAIL;
                goto cleanup;
            }

            error_check = get_reg_value(temp_register, &temp);
            if(-1 == error_check){
                return_value = FAIL;
                goto cleanup;
            }

            if(first || (TOKEN == instructions[reg_struct.etp-1].token_type && ADD == instructions[reg_struct.etp-1].data.token)){
                return_value += temp;
            }
            else if(TOKEN == instructions[reg_struct.etp-1].token_type && SUB == instructions[reg_struct.etp-1].data.token){
                return_value -= temp;
            }
            else{
                printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: unexpected token value\n", get_curr_line(source));
                return_value = FAIL;
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
                printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: unexpected token value\n", get_curr_line(source));
                return_value = FAIL;
                goto cleanup;
            }
        }

        else if(STRING == instructions[reg_struct.etp].token_type){
            temp = (long int)get_var_value(instructions[reg_struct.etp].data.str);
            if(first || (TOKEN == instructions[reg_struct.etp-1].token_type && ADD == instructions[reg_struct.etp-1].data.token)){
                return_value += temp;
            }
            else if(TOKEN == instructions[reg_struct.etp-1].token_type && SUB == instructions[reg_struct.etp-1].data.token){
                return_value -= temp;
            }
            else{
                printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: unexpected token value\n", get_curr_line(source));
                return_value = FAIL;
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

int execute_instructions(FILE * source){
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
        printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: expected token\n", get_curr_line(source));
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
                printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: expected register or integral type\n", get_curr_line(source));
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
                        printf("\e[31mError\e[0m invalid register size on line \e[31m%i\e[0m", get_curr_line(source));
                        return_value = -1;
                        goto cleanup;
                }
            }
            else if(NONE == instructions[reg_struct.etp].token_type){
                reg_struct.rsp.reg_64 -= STACK_ELEMENT_SIZE;
            }
            else{
                printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: expected register type\n", get_curr_line(source));
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
                    printf("\e[31mError\e[0m on line \e[31m%i\e[0m: Expected size token\n", get_curr_line(source));
                    return_value = -1;
                    goto cleanup;
                }

                //Get p1 
                reg_struct.etp ++;
                r1 = get_pointer_value(source);
                if(FAIL == r1){
                    puts("EXECUTE_INSTRUCTIONS: Get_pointer_value error");
                    return_value = -1;
                    goto cleanup;
                }

                //printf("%i TOKEN VALUE: %i\n", reg_struct.etp, instructions[reg_struct.etp].token_type);

                if(TOKEN == instructions[reg_struct.etp].token_type && IN == instructions[reg_struct.etp].data.token){    //MOV [p1] [p2] (eg. MOV [rax] [rbp - 16])
                    r2 = get_pointer_value(source);
                    if(FAIL == r2){
                        puts("EXECUTE_INSTRUCTIONS: Get_pointer_value error");
                        return_value = -1;
                        goto cleanup;
                    }
                    
                    switch(size){
                        case BYTE: *(char *)r1 = *(char *)r2; break;
                        case WORD: *(short int *)r1 = *(short int *)r2; break;
                        case DWORD: *(int *)r1 = *(int *)r2; break;
                        case QWORD: *(long int *)r1 = *(long int *)r2; break;
                        default: printf("\e[31mError\e[0m on line %i: Invalid size value", get_curr_line(source)); return_value = -1; goto cleanup;
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
                        default: printf("\e[31mError\e[0m on line %i: Invalid size value", get_curr_line(source)); return_value = -1; goto cleanup;
                    }
                }
                else if(NUM == instructions[reg_struct.etp].token_type){        //MOV [p1] NUM [eg. MOV [rax] 121]
                    temp = instructions[reg_struct.etp].data.num;

                    switch(size){
                        case BYTE: *(char *)r1 = (char)temp; break;
                        case WORD: *(short int *)r1 = (short int)temp; break;
                        case DWORD: *(int *)r1 = (int)temp; break;
                        case QWORD: *(long int *)r1 = (long int)temp; break;
                        default: printf("\e[31mError\e[0m on line %i: Invalid size value", get_curr_line(source)); return_value = -1; goto cleanup;
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
                        printf("\e[31mError\e[0m on line \e[31m%i\e[0m: Expected size token\n", get_curr_line(source));
                        return_value = -1;
                        goto cleanup;
                    }

                    reg_struct.etp ++;
                    r1 = get_pointer_value(source);
                    if(FAIL == r1){
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
                    printf("\e[31mError\e[0m on line \e[31m%i\e[0m: LEA must be followed by a [ token\n", get_curr_line(source));
                    return_value = -1;
                    goto cleanup;
                }

                temp = (long int)get_pointer_value(source);
                if(-1 == temp){
                    return_value = -1;
                    goto cleanup;
                }

                return_value = set_reg_value(r3, temp, i);
                if(-1 == return_value){
                    goto cleanup;
                }
            }
            else{
                printf("\e[31mError\e[0m on line \e[31m%i\e[0m: expected register as first parameter\n", get_curr_line(source));
                return_value = -1;
                goto cleanup;
            }

            break;
        }
    
        case TAG: {
            reg_struct.etp ++;
            if(STRING != instructions[reg_struct.etp].token_type){
                printf("\e[31mError\e[0m on line \e[31m%i\e[0m: expected string\n", get_curr_line(source));
                return_value = -1;
                goto cleanup;
            }

            temp = ftell(source);
            if(-1 == temp){
                return_value = print_error("\e[31mEXECUTE_INSTRUCTIONS\e[0m: ftell error", -1);
                goto cleanup;
            }


            return_value = insert_jump_offset(instructions[reg_struct.etp].data.str, (int)temp);
            if(-1 == return_value){
                goto cleanup;
            }

            break;
        }

        case JMP: {
            reg_struct.etp ++;
            return_value = jump(source);
            if(-1 == return_value){
                goto cleanup;
            }

            break;
        }

        case CALL: {
            reg_struct.etp ++;
            if(STRING != instructions[reg_struct.etp].token_type){
                printf("\e[31mError\e[0m on line \e[31m%i\e[0m: expected string\n", get_curr_line(source));
                return_value = -1;
                goto cleanup;
            }

            temp = strncmp(instructions[reg_struct.etp].data.str, "WRITE", BUFFER_SIZE);
            if(0 == temp){
                reg_struct.rax.reg_64 = write(reg_struct.rax.reg_32[0], (void *)reg_struct.rbx.reg_64, reg_struct.rcx.reg_64);
                break;
            }
            
            temp = strncmp(instructions[reg_struct.etp].data.str, "READ", BUFFER_SIZE);
            if(0 == temp){
                reg_struct.rax.reg_64 = read(reg_struct.rax.reg_32[0], (void *)reg_struct.rbx.reg_64, reg_struct.rcx.reg_64);
                break;
            }

            temp = strncmp(instructions[reg_struct.etp].data.str, "OPEN", BUFFER_SIZE);
            if(0 == temp){
                temp = 0;

                for(i=0; i<reg_struct.rbx.reg_64; i++){
                    reg_struct.rsp.reg_64 -= STACK_ELEMENT_SIZE;
                    temp |= *(long int *)reg_struct.rsp.reg_64;
                }

                reg_struct.rax.reg_32[0] = open((const char *)reg_struct.rax.reg_64, temp);

                break;
            }

            temp = get_jump_offset(instructions[reg_struct.etp].data.str);
            if(-1 == temp){
                temp = find_tag(source, instructions[reg_struct.etp].data.str);
                if(-1 == temp){
                    return_value = -1;
                    goto cleanup;
                }
            }

            *(long int *)reg_struct.rsp.reg_64 = reg_struct.rip.reg_64;
            reg_struct.rsp.reg_64 += STACK_ELEMENT_SIZE;

            return_value = rseek(source, temp, SEEK_SET);
            if(-1 == return_value){
                return_value = print_error("\e[31mEXECUTE_INSTRUCTIONS\e[0m: Fseek error", -1);
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
                printf("\e[31mError\e[0m on line \e[31m%i\e[0m: expected register or integral type\n", get_curr_line(source));
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
                printf("\e[31mError\e[0m on line \e[31m%i\e[0m: expected register or integral type\n", get_curr_line(source));
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
                return_value = jump(source);
                if(-1 == return_value){
                    goto cleanup;
                }
            }

            break;
        }

        case JLE: {
            reg_struct.etp ++;
            if(flags.zf || flags.sf){
                return_value = jump(source);
                if(-1 == return_value){
                    goto cleanup;
                }
            }

            break;
        }

        case JGE: {
            reg_struct.etp ++;
            if(flags.zf || !flags.sf){
                return_value = jump(source);
                if(-1 == return_value){
                    goto cleanup;
                }
            }

            break;
        }

        case JL: {
            reg_struct.etp ++;
            if(!flags.zf && flags.sf){
                return_value = jump(source);
                if(-1 == return_value){
                    goto cleanup;
                }
            }

            break;
        }

        case JG: {
            reg_struct.etp ++;
            if(!flags.zf && !flags.sf){
                return_value = jump(source);
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
                printf("\e[31mError\e[0m on line \e[31m%i\e[0m: Expected register value\n", get_curr_line(source));
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

        case SET: {
            reg_struct.etp ++;
            return_value = set_text_var();
            if(-1 == return_value){
                goto cleanup;
            }

            break;
        }

        default: printf("\e[33mWarning\e[0m on line \e[33m%i\e[0m: unsupported token. (These may be added in later versions)\n", get_curr_line(source)); goto cleanup;
    }

    reg_struct.etp ++;

    for(; reg_struct.etp < INSTRUCTION_SIZE; reg_struct.etp++){
        if(instructions[reg_struct.etp].token_type != NONE){
            printf("\e[33mWarning\e[0m on line \e[33m%i\e[0m: excess tokens (index: \e[33m%i\e[0m)\n(Excess tokens are ignored)\n", get_curr_line(source), reg_struct.etp);
            printf("TOKEN TYPE: %i TOKEN VALUE: %li\n", instructions[reg_struct.etp].token_type, instructions[reg_struct.etp].data.num);
            //break;
        }
    }

cleanup:
    return return_value;
}

int execute(char * filename, func_flags_t fun_flags){
    int return_value = 0;
    int i = 0;
    off_t entry_off = 0;
    off_t length = 0;
    FILE * source = NULL;

    source = fopen(filename, "r");
    if(NULL == source){
        return_value = print_error("\e[31mEXECUTE: Fopen error\e[0m", -1);
        goto cleanup;
    }

    entry_off = find_tag(source, ENTRY_POINT);
    if(-1 == entry_off){
        return_value = -1;
        goto cleanup;
    }

    return_value = fseek(source, 0, SEEK_END);
    if(-1 == return_value){
        return_value = print_error("\e[31mEXECUTE: Fseek error\e[0m", -1);
        goto cleanup;
    }
    length = ftell(source);
    if(-1 == length){
        length = print_error("\e[31mEXECUTE: Ftell error\e[0m", -1);
        goto cleanup;
    }

    return_value = rseek(source, entry_off, SEEK_SET);
    if(-1 == return_value){
        return_value = print_error("\e[31mEXECUTE: Fseek error\e[0m", -1);
        goto cleanup;
    }

    while(reg_struct.rip.reg_64 <= length){
        return_value = get_next_sequence(source);
        if(-1 == return_value){
            goto cleanup;
        }

        if(fun_flags.print_instructions){
            print_instructions();
        }
        return_value = execute_instructions(source);
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

        for(i=0; i<INSTRUCTION_SIZE; i++){
            if(STRING == instructions[i].token_type){
                free(instructions[i].data.str);
            }
        }
        memset(instructions, 0, sizeof(instruction_t) * INSTRUCTION_SIZE);
        reg_struct.etp = 0;

        if(feof(source) && reg_struct.rip.reg_64 <= length){
            clearerr(source);
        }
    }

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

    munmap(stack, getpagesize());
    munmap(text, getpagesize());

cleanup:
    return error_check;
}
