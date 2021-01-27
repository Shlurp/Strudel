#include "inter.h"

char * tokens[] = {"PUSH", "POP", "MOV", "JMP", "CMP", "JE", "JNE", "JG", "JGE", "JL", "JLE", "ADD", "SUB", "SET"};
char * regs[][5] = {{"RSP"}, 
                    {"RBP"}, 
                    {"RAX", "EAX", "AX", "AH", "AL"}, 
                    {"RBX", "EBX", "BX", "BH", "BL"}, 
                    {"RCX", "ECX", "CX", "CH", "CL"}, 
                    {"RDX", "EDX", "DX", "DH", "DL"}};

static inline int get_reg(union reg_u ** reg){
    int return_value = 0;

    switch(instructions[reg_struct.etp].data.reg.reg){ 
        case RSP: *reg = &reg_struct.rsp; break; 
        case RBP: *reg = &reg_struct.rbp; break;
        case RAX: *reg = &reg_struct.rax; break; 
        case RBX: *reg = &reg_struct.rax; break; 
        case RCX: *reg = &reg_struct.rax; break; 
        case RDX: *reg = &reg_struct.rax; break; 
        default: 
            if(0 <= instructions[reg_struct.etp].data.reg.reg && instructions[reg_struct.etp].data.reg.reg < NUM_REG_SIZE){ 
                *reg = &reg_struct.rx[instructions[reg_struct.etp].data.reg.reg]; 
                break; 
            } 
            else{ 
                printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: unexpected register value\n", line); 
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

    if(instructions[reg_struct.etp].data.reg.size != R_REG_SIZE){
        for(int j=0; j<8; j++){
            printf("%i\n", reg->reg_8[j]);
        }
    }

    switch(instructions[reg_struct.etp].data.reg.size){
        case R_REG_SIZE: *value = reg->reg_64; break;
        case E_REG_SIZE: *value = reg->reg_32[i]; break;
        case X_REG_SIZE: *value = reg->reg_16[i]; break;
        case HL_REG_SIZE: *value = reg->reg_8[i]; break;
        default:
            printf("\e[31mError\e[0m invalid register size on line \e[31m%i\e[0m", line);
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
            printf("\e[31mError\e[0m invalid register size on line \e[31m%i\e[0m\n", line);
            return_value = -1;
            goto cleanup;
    }

cleanup:
    return return_value;
}

/**
 * djb2 hash algorithm by Dan Bernstein: http://www.cse.yorku.ca/~oz/hash.html
 */
unsigned long hash(unsigned char *str){
    unsigned long hash = 5381;
    int c = 0;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

int get_token_str(FILE * source, char ** token_str){
    int return_value = 0;
    int buffer_pointer = 0;
    char curr_char = 0;
    bool_t first = true;
    char buffer[BUFFER_SIZE] = {0};

    while(true){
        return_value = fread(&curr_char, sizeof(curr_char), 1, source);
        if(0 == return_value && !feof(source)){
            return_value = print_error("\e[31mGET_TOKEN_STR: Fread error\e[0m", -1);
            goto cleanup;
        }
        else if((feof(source)) || ((' ' == curr_char || ',' == curr_char) && !first)){
            break;
        }
        if('\n' == curr_char && !first){
            newline = true;
            line ++;
            break;
        }
        else if(';' == curr_char){
            while('\n' != curr_char && !feof(source)){
                return_value = fread(&curr_char, sizeof(curr_char), 1, source);
                if(0 == return_value && !feof(source)){
                    return_value = print_error("\e[31mGET_TOKEN_STR: Fread error\e[0m", -1);
                    goto cleanup;
                }
            }
            if(!first){
                newline = true;
                line ++;
            }
            break;
        }
        else if('\n' == curr_char && first){
            line ++;
            continue;
        }
        else if((' ' == curr_char || ',' == curr_char) && first){
            continue;
        }

        if(']' == curr_char && !first){
            return_value = fseek(source, -1, SEEK_CUR);
            if(-1 == return_value){
                print_error("\e[31mGET_TOKEN_STR: lseek error\e[0m", 0);
                goto cleanup;
            }
            break;
        } 

        first = false;

        buffer[buffer_pointer] = curr_char;

        if('[' == curr_char || ']' == curr_char){
            return_value = fread(&curr_char, sizeof(curr_char), 1, source);
            if(0 == return_value && !feof(source)){
                return_value = print_error("\e[31mGET_TOKEN_STR: Fread error\e[0m", -1);
                goto cleanup;
            }

            if('\n' == curr_char){
                newline = true;
                line ++;
            }
            else{
                return_value = fseek(source, -1, SEEK_CUR);
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
    char * endptr = NULL;
    char * token_str = NULL;

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
                 *                    R*X
                 *   _______________________________________
                 *  /                           E*X         \
                 *  |                    __________________ |
                 *  |                   /            *X    \|
                 *  |                   |          _______ ||
                 *  |                   |         /       \||
                 *  +----+----+----+----+----+----+----+----+
                 *  |    |    |    |    |    |    | *H | *L |
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
                    printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: token: \e[31m%s\e[0m matches with register \e[31m%s\e[0m, but is not a valid register\nExiting...\n", line+1, token_str, regs[i][j]);
                    return_value = -1;
                    goto cleanup;
                }
                goto cleanup;
            }
        }
    }

    if('-' == token_str[0]){
        i = 1;
    }
    else{
        i = 0;
    }
    for(i; i<token_length; i++){
        if(token_str[i] < '0' || token_str[i] > '9'){
            printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m on token: \e[31m%s\e[0m on index: \e[31m%i\e[0m\nExiting...\n", line+1, token_str, i);
            return_value = -1;
            goto cleanup;
        }

        instruction->token_type = NUM;
        instruction->data.num = instruction->data.num * 10 + token_str[i] - '0';
    }

    if('-' == token_str[0]){
        instruction->data.num *= -1;
    }

cleanup:
    if(NULL != token_str){
        free(token_str);
    }
    return return_value;
}

int get_next_sequence(FILE * source){
    int return_value = 0;
    int i = 0;

    for(i=0; i<sizeof(instructions)/sizeof(instructions[0]) && !newline && !feof(source); i++){
        return_value = get_next_instruction(source, &(instructions[i]));
        if(-1 == return_value){
            goto cleanup;
        }
    }
    newline = false;

cleanup:
    return return_value;
}

int * get_pointer_value(){
    char * return_value = 0;
    int error_check = 0;
    long int temp = 0;
    int i = 0;
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
            i = instructions[reg_struct.etp].data.reg.index;

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
                printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: unexpected token value\n", line);
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
                printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: unexpected token value\n", line);
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

int execute_instructions(){
    int return_value = 0;
    long int temp = 0;
    int i = 0;
    int * r1 = NULL;
    int * r2 = NULL;
    union reg_u * r3 = NULL;
    union reg_u * r4 = NULL;
    union reg_u * temp_register = NULL;

    if(TOKEN != instructions[0].token_type){
        printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: expected token\n", line);
        return_value = -1;
        goto cleanup;
    }

    switch(instructions[0].data.token){
        case PUSH:
            if(NUM == instructions[1].token_type){
                *((int *)reg_struct.rsp.reg_64) = instructions[1].data.num;
            }
            else if(REGISTER == instructions[1].token_type){
                reg_struct.etp = 1;

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
                printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: expected register or integral type\n", line);
                return_value = -1;
                goto cleanup;
            }

            reg_struct.rsp.reg_64 += STACK_ELEMENT_SIZE;
            break;

        case POP:
            if(REGISTER != instructions[1].token_type){
                printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: expected register type\n", line);
                return_value = -1;
                goto cleanup;
            }

            reg_struct.rsp.reg_64 -= STACK_ELEMENT_SIZE;

            reg_struct.etp = 1;

            return_value = get_reg(&temp_register);
            if(-1 == return_value){
                goto cleanup;
            }

            switch(instructions[1].data.reg.size){
                case R_REG_SIZE: temp_register->reg_64 = *(long int *)reg_struct.rsp.reg_64; break;
                case E_REG_SIZE: temp_register->reg_32[i] = *(long int *)reg_struct.rsp.reg_64; break;
                case X_REG_SIZE: temp_register->reg_16[i] = *(long int *)reg_struct.rsp.reg_64; break;
                case HL_REG_SIZE: temp_register->reg_8[i] = *(long int *)reg_struct.rsp.reg_64; break;
                default:
                    printf("\e[31mError\e[0m invalid register size on line \e[31m%i\e[0m", line);
                    return_value = -1;
                    goto cleanup;
            }

            break;

        case MOV:
            if(TOKEN == instructions[1].token_type && IN == instructions[1].data.token){        //MOV [p1] ____
                //Get p1 
                reg_struct.etp = 2;
                r1 = get_pointer_value();
                if(FAIL == r1){
                    puts("EXECUTE_INSTRUCTIONS: Get_pointer_value error");
                    return_value = -1;
                    goto cleanup;
                }

                //printf("%i TOKEN VALUE: %i\n", reg_struct.etp, instructions[reg_struct.etp].token_type);

                if(TOKEN == instructions[reg_struct.etp].token_type && IN == instructions[reg_struct.etp].data.token){    //MOV [p1] [p2] (eg. MOV [rax] [rbp - 16])
                    r2 = get_pointer_value();
                    if(FAIL == r2){
                        puts("EXECUTE_INSTRUCTIONS: Get_pointer_value error");
                        return_value = -1;
                        goto cleanup;
                    }

                    *r1 = *r2;
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

                    *r1 = temp;
                }
                else if(NUM == instructions[reg_struct.etp].token_type){        //MOV [p1] NUM [eg. MOV [rax] 121]
                    r2 = get_pointer_value();
                    if(FAIL == r1){
                        puts("EXECUTE_INSTRUCTIONS: Get_pointer_value error");
                        return_value = -1;
                        goto cleanup;
                    }

                    *(int *)r1 = (long int)r2;
                }
            }
            else if(REGISTER == instructions[1].token_type){        //MOV reg ____
                reg_struct.etp = 1;
                i = 1;
                return_value = get_reg(&r3);
                if(-1 == return_value){
                    goto cleanup;
                }

                if(TOKEN == instructions[2].token_type && IN == instructions[2].data.token){        //MOV reg [p1] (eg. MOV rax [rbp + 16])
                    reg_struct.etp = 3;
                    r1 = get_pointer_value();
                    if(FAIL == r1){
                        puts("EXECUTE_INSTRUCTIONS: Get_pointer_value error");
                        return_value = -1;
                        goto cleanup;
                    }

                    return_value = set_reg_value(r3, *(long int *)r1, i);
                    if(-1 == return_value){
                        goto cleanup;
                    }
                }
                else if(REGISTER == instructions[2].token_type){    //MOV reg1 reg2 (eg. MOV rax rcx)
                    reg_struct.etp = 2;
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
                else if(NUM == instructions[2].token_type){     
                    reg_struct.etp = 2;

                    r1 = get_pointer_value();
                    if(FAIL == r1){
                        puts("EXECUTE_INSTRUCTIONS: Get_pointer_value error");
                        return_value = -1;
                        goto cleanup;
                    }
                    //printf("VALUE: %li\n", (long int)r1);

                    return_value = set_reg_value(r3, (long int)r1, i);
                }
            }

            break;
    }

cleanup:
    return return_value;
}

int execute(char * filename){
    int return_value = 0;
    FILE * source = NULL;
    long int i = 0;
    int j = 0;

    source = fopen(filename, "r");
    if(NULL == source){
        return_value = print_error("\e[31mEXECUTE: Fopen error\e[0m", -1);
        goto cleanup;
    }

    while(!feof(source)){
        return_value = get_next_sequence(source);
        if(-1 == return_value){
            goto cleanup;
        }

        //print_instructions();
        return_value = execute_instructions();
        if(-1 ==return_value){
            goto cleanup;
        }
        print_regs();

        memset(instructions, 0, sizeof(instructions));
        reg_struct.etp = 0;

        for(i=(long int)stack; i<=reg_struct.rsp.reg_64; i+=STACK_ELEMENT_SIZE){
            printf("%p: %i", (void *)i, *(int *)i);

            if(i <= reg_struct.rsp.reg_64 && reg_struct.rsp.reg_64 < i + STACK_ELEMENT_SIZE){
                fputs("\e[32m < RSP\e[0m", stdout);
            }
            if(i <= reg_struct.rbp.reg_64 && reg_struct.rbp.reg_64 < i + STACK_ELEMENT_SIZE){
                fputs("\e[32m < RBP\e[0m", stdout);
            }
            if(i <= reg_struct.rax.reg_64 && reg_struct.rax.reg_64 < i + STACK_ELEMENT_SIZE){
                fputs("\e[32m < RAX\e[0m", stdout);
            }
            if(i <= reg_struct.rcx.reg_64 && reg_struct.rcx.reg_64 < i + STACK_ELEMENT_SIZE){
                fputs("\e[32m < RCX\e[0m", stdout);
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

cleanup:
    if(NULL != source){
        fclose(source);
    }

    return return_value;
}

int main(int argc, char ** argv){
    int error_check = 0;

    error_check = init();
    if(-1 == error_check){
        goto cleanup;
    }

    execute(argv[1]);

    munmap(stack, getpagesize());
    munmap(text, getpagesize());

cleanup:
    return error_check;
}