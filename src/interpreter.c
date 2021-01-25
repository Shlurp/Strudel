#include "inter.h"

char * tokens[] = {"PUSH", "POP", "MOV", "JMP", "CMP", "JE", "JNE", "JG", "JGE", "JL", "JLE", "ADD", "SUB", "SET"};
char * regs[] = {"RSP", "RBP", "RAX", "RCX"};

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
        else if((return_value == 0 && feof(source)) || (' ' == curr_char && !first)){
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
            newline = true;
            break;
        }
        else if('\n' == curr_char && first){
            line ++;
            continue;
        }
        else if(' ' == curr_char && first){
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

    for(i=0; i<sizeof(tokens)/sizeof(tokens[0]); i++){
        difference = strncmp(token_str, tokens[i], BUFFER_SIZE);
        if(0 == difference){
            instruction->token_type = TOKEN;
            instruction->data.token = i+1;
            goto cleanup;
        }
    }

    for(i=0; i<sizeof(regs)/sizeof(regs[0]); i++){
        difference = strncmp(token_str, regs[i], BUFFER_SIZE);
        if(0 == difference){
            instruction->token_type = REGISTER;
            instruction->data.reg = i+1;
            goto cleanup;
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
            printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m on token: \e[31m%s\e[0m on index: \e[31m%i\e[0m\nExiting...\n", line, token_str, i);
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
            puts("GET_NEXT_SEQUENCE: Get_next_instruction error");
            goto cleanup;
        }
    }
    newline = false;

cleanup:
    return return_value;
}

int * get_pointer_value(){
    char * return_value = 0;
    bool_t first = true;

    while( !(TOKEN == instructions[register_struct.etp].token_type && 
            OUT == instructions[register_struct.etp].data.token) &&  
            NONE != instructions[register_struct.etp].data.token){

        //printf("TOKEN TYPE: %i\nVALUE: %i\n", instructions[register_struct.etp].token_type, instructions[register_struct.etp].data.token);
            
        if(TOKEN == instructions[register_struct.etp].token_type && IN == instructions[register_struct.etp].data.token){
            register_struct.etp ++;
            continue;
        }

        if(REGISTER == instructions[register_struct.etp].token_type){
            switch(instructions[register_struct.etp].data.reg){
                case RSP:
                    if(first || (TOKEN == instructions[register_struct.etp-1].token_type && ADD == instructions[register_struct.etp-1].data.token)){
                        return_value += register_struct.rsp;
                    }
                    else if(TOKEN == instructions[register_struct.etp-1].token_type && SUB == instructions[register_struct.etp-1].data.token){
                        return_value -= register_struct.rsp;
                    }
                    break;
                case RBP:
                    if(first || (TOKEN == instructions[register_struct.etp-1].token_type && ADD == instructions[register_struct.etp-1].data.token)){
                        return_value += register_struct.rbp;
                    }
                    else if(TOKEN == instructions[register_struct.etp-1].token_type && SUB == instructions[register_struct.etp-1].data.token){
                        return_value -= register_struct.rbp;
                    }
                    break;
                case RAX:
                    if(first || (TOKEN == instructions[register_struct.etp-1].token_type && ADD == instructions[register_struct.etp-1].data.token)){
                        return_value += register_struct.rax;
                    }
                    else if(TOKEN == instructions[register_struct.etp-1].token_type && SUB == instructions[register_struct.etp-1].data.token){
                        return_value -= register_struct.rax;
                    }
                    break;
                case RCX:
                    if(first || (TOKEN == instructions[register_struct.etp-1].token_type && ADD == instructions[register_struct.etp-1].data.token)){
                        return_value += register_struct.rcx;
                    }
                    else if(TOKEN == instructions[register_struct.etp-1].token_type && SUB == instructions[register_struct.etp-1].data.token){
                        return_value -= register_struct.rcx;
                    }
                    break;

                default:
                    printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: unexpected register value\n", line-1);
                    return_value = FAIL;
                    goto cleanup;
            }
        }
        else if(NUM == instructions[register_struct.etp].token_type){
            if(first || (TOKEN == instructions[register_struct.etp-1].token_type && ADD == instructions[register_struct.etp-1].data.token)){
                return_value += instructions[register_struct.etp].data.num;
            }
            else if(TOKEN == instructions[register_struct.etp-1].token_type && SUB == instructions[register_struct.etp-1].data.token){
                return_value -= instructions[register_struct.etp].data.num;
            } 
        }

        register_struct.etp ++;
        first = false;
    }

    if(TOKEN == instructions[register_struct.etp].token_type && OUT == instructions[register_struct.etp].data.token){
        register_struct.etp ++;
    }

    //printf("VALUE: %p %p\n", return_value, (void *)(register_struct.rsp-8));

cleanup:
    return (int *)return_value;
}

int execute_instructions(){
    int return_value = 0;
    int * r1 = NULL;
    int * r2 = NULL;
    long int * r3 = NULL;
    long int * r4 = NULL;

    if(TOKEN != instructions[0].token_type){
        printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: expected token\n", line-1);
        return_value = -1;
        goto cleanup;
    }

    switch(instructions[0].data.token){
        case PUSH:
            if(NUM == instructions[1].token_type){
                *((int *)register_struct.rsp) = instructions[1].data.num;
            }
            else if(REGISTER == instructions[1].token_type){
                switch(instructions[1].data.reg){
                    case RSP:
                        *((int *)register_struct.rsp) = register_struct.rsp;
                        break;
                    case RBP:
                        *((int *)register_struct.rsp) = register_struct.rbp;
                        break;
                    case RAX:
                        *((int *)register_struct.rsp) = register_struct.rax;
                        break;
                    case RCX:
                        *((int *)register_struct.rsp) = register_struct.rcx;
                        break;

                    default:
                        printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: unexpected register value\n", line-1);
                        return_value = -1;
                        goto cleanup;
                }
            }
            else{
                printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: expected register or integral type\n", line-1);
                return_value = -1;
                goto cleanup;
            }

            register_struct.rsp += sizeof(int);
            break;

        case POP:
            if(REGISTER != instructions[1].token_type){
                printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: expected register type\n", line-1);
                return_value = -1;
                goto cleanup;
            }

            register_struct.rsp -= sizeof(int);

            switch(instructions[1].data.reg){
                case RSP:
                    register_struct.rsp = *((int *)register_struct.rsp);
                    break;
                case RBP:
                    register_struct.rbp = *((int *)register_struct.rsp);
                    break;
                case RAX:
                    register_struct.rax = *((int *)register_struct.rsp);
                    break;
                case RCX:
                    register_struct.rcx = *((int *)register_struct.rsp);
                    break;
                    
                default:
                    printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: unexpected register value\n", line-1);
                    return_value = -1;
                    goto cleanup;
            }

            break;

        case MOV:
            if(TOKEN == instructions[1].token_type && IN == instructions[1].data.token){        //MOV [p1] ____
                //Get p1 
                register_struct.etp = 2;
                r1 = get_pointer_value();
                if(FAIL == r1){
                    puts("EXECUTE_INSTRUCTIONS: Get_pointer_value error");
                    return_value = -1;
                    goto cleanup;
                }

                //printf("%i TOKEN VALUE: %i\n", register_struct.etp, instructions[register_struct.etp].token_type);

                if(TOKEN == instructions[register_struct.etp].token_type && IN == instructions[register_struct.etp].data.token){    //MOV [p1] [p2] (eg. MOV [rax] [rbp - 16])
                    r2 = get_pointer_value();
                    if(FAIL == r2){
                        puts("EXECUTE_INSTRUCTIONS: Get_pointer_value error");
                        return_value = -1;
                        goto cleanup;
                    }

                    *r1 = *r2;
                }
                else if(REGISTER == instructions[register_struct.etp].token_type){      //MOV [p1] reg (eg. MOV [rax] rcx)
                    switch(instructions[register_struct.etp].data.reg){
                        case RSP:
                            *r1 = register_struct.rsp;
                            break;
                        case RBP:
                            *r1 = register_struct.rbp;
                            break;
                        case RAX:
                            *r1 = register_struct.rax;
                            break;
                        case RCX:
                            *r1 = register_struct.rcx;
                            break;

                        default:
                            printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: unexpected register value\n", line-1);
                            return_value = -1;
                            goto cleanup;
                    }
                }
                else if(NUM == instructions[register_struct.etp].token_type){
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
                switch(instructions[1].data.reg){
                    case RSP:
                        r3 = &register_struct.rsp;
                        break;
                    case RBP:
                        r3 = &register_struct.rbp;
                        break;
                    case RAX:
                        r3 = &register_struct.rax;
                        break;
                    case RCX:
                        r3 = &register_struct.rcx;
                        break;
                    default:
                        printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: unexpected register value\n", line-1);
                        return_value = -1;
                        goto cleanup;
                }

                if(TOKEN == instructions[2].token_type && IN == instructions[2].data.token){        //MOV reg [p1] (eg. MOV rax [rbp + 16])
                    register_struct.etp = 3;
                    r1 = get_pointer_value();
                    if(FAIL == r1){
                        puts("EXECUTE_INSTRUCTIONS: Get_pointer_value error");
                        return_value = -1;
                        goto cleanup;
                    }

                    *r3 = *r1;
                }
                else if(REGISTER == instructions[2].token_type){    //MOV reg1 reg2 (eg. MOV rax rcx)
                    switch(instructions[2].data.reg){
                        case RSP:
                            r4 = &register_struct.rsp;
                            break;
                        case RBP:
                            r4 = &register_struct.rbp;
                            break;
                        case RAX:
                            r4 = &register_struct.rax;
                            break;
                        case RCX:
                            r4 = &register_struct.rcx;
                            break;

                        default:
                            printf("\e[31;1mError\e[0m on line \e[31m%i\e[0m: unexpected register value\n", line-1);
                            return_value = -1;
                            goto cleanup;
                    }

                    *r3 = *r4;
                }
                else if(NUM == instructions[2].token_type){     
                    register_struct.etp = 2;

                    puts("\e[31mNUM\e[0m");

                    r1 = get_pointer_value();
                    if(FAIL == r1){
                        puts("EXECUTE_INSTRUCTIONS: Get_pointer_value error");
                        return_value = -1;
                        goto cleanup;
                    }
                    //printf("VALUE: %li\n", (long int)r1);

                    *r3 = (long int)r1;
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

    source = fopen(filename, "r");
    if(NULL == source){
        return_value = print_error("\e[31mEXECUTE: Fopen error\e[0m", -1);
        goto cleanup;
    }

    while(!feof(source)){
        get_next_sequence(source);
        for(i=0; i<16; i++){
            if(instructions[i].token_type != NONE){
                //printf("TOKEN TYPE: %i\nDATA: %i\n\n", instructions[i].token_type, instructions[i].data.num);
            }
        }
        execute_instructions();

        memset(instructions, 0, sizeof(instructions));
        register_struct.etp = 0;

        for(i=(long int)stack; i<=register_struct.rsp; i+=sizeof(int)){
            printf("%p: %i", (void *)i, *(int *)i);

            if(i <= register_struct.rsp && register_struct.rsp < i + sizeof(int)){
                fputs("\e[32m < RSP\e[0m", stdout);
            }
            if(i <= register_struct.rbp && register_struct.rbp < i + sizeof(int)){
                fputs("\e[32m < RBP\e[0m", stdout);
            }
            if(i <= register_struct.rax && register_struct.rax < i + sizeof(int)){
                fputs("\e[32m < RAX\e[0m", stdout);
            }
            if(i <= register_struct.rcx && register_struct.rcx < i + sizeof(int)){
                fputs("\e[32m < RCX\e[0m", stdout);
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