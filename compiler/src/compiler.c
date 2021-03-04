#include "comp.h"

#define FILE_NAME_LEN 16

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
char * functions[] = {"OPEN", "READ", "WRITE", "PRNUM"};

int errnum = 0;
registers_t reg_struct = {0};
instruction_t instructions[INSTRUCTION_SIZE] = {0};
bool_t newline = false;

int get_token_str(file_t * source, char ** token_str, int * line_no){
    int return_value = 0;
    int buffer_pointer = 0;
    char curr_char = 0;
    bool_t first = true;
    bool_t str = false;
    char buffer[BUFFER_SIZE] = {0};

    while(true){
        curr_char = MF_getc(source);
        if((MF_eof(source)) || ((' ' == curr_char || ',' == curr_char) && !first && !str)){
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
                while('\n' != curr_char && !MF_eof(source)){
                    curr_char = MF_getc(source);
                    if(EOF == curr_char && !MF_eof(source)){
                        return_value = print_error("\e[31mGET_TOKEN_STR: Fread error\e[0m", -1);
                        goto cleanup;
                    }
                }
                
                if(0 == reg_struct.etp){
                    (*line_no) ++;
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
            (*line_no) ++;
            continue;
        }

        if(']' == curr_char && !first && !str){
            return_value = MF_ungetc(source);
            if(-1 == return_value){
                print_error("\e[31mGET_TOKEN_STR: Ungetc error\e[0m", 0);
                goto cleanup;
            }
            break;
        } 

        first = false;

        buffer[buffer_pointer] = curr_char;

        if(('[' == curr_char || ']' == curr_char) && !str){
            curr_char = MF_getc(source);
            if(EOF == curr_char && !MF_eof(source)){
                return_value = print_error("\e[31mGET_TOKEN_STR: Fread error\e[0m", -1);
                goto cleanup;
            }
            else if(MF_eof(source)){
                break;
            }

            if('\n' == curr_char){
                newline = true;
            }
            else{
                return_value = MF_ungetc(source);
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

int get_next_instruction(file_t * source, instruction_t * instruction, int * line_no){
    int return_value = 0;
    int token_length = 0;
    int difference = 0;
    int i = 0;
    int j = 0;
    char * token_str = NULL;

    token_length = get_token_str(source, &token_str, line_no);
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
                    printf("\e[31;1mError\e[0m: token: \e[31m%s\e[0m matches with register \e[31m%s\e[0m, but is not a valid register\nExiting...\n", token_str, regs[i][j]);
                    return_value = -1;
                    goto cleanup;
                }
                goto cleanup;
            }
        }
    }

    for(i=0; i<sizeof(functions)/sizeof(functions[0]); i++){
        difference = strncmp(token_str, functions[i], BUFFER_SIZE);
        if(0 == difference){
            instruction->token_type = FUNCTION;
            instruction->data.function = i + 1;
            goto cleanup;
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

int get_next_sequence(file_t * source, int * line_no){
    int return_value = 0;

    memset(instructions, 0, sizeof(instructions));
    for(reg_struct.etp=0; reg_struct.etp<INSTRUCTION_SIZE && !newline && !MF_eof(source); reg_struct.etp++){
        return_value = get_next_instruction(source, &(instructions[reg_struct.etp]), line_no);
        if(-1 == return_value){
            goto cleanup;
        }
    }
    newline = false;

    reg_struct.etp = 0;

cleanup:
    return return_value;
}

int manage_sequence(file_t * source, int compiled_fd, int * line_no, func_flags_t fun_flags){
    int error_check = 0;
    off_t offset = 0;
    long int value = 0;
    variable_t * var = NULL;

    error_check = get_next_sequence(source, line_no);
    if(-1 == error_check){
        goto cleanup;
    }

    if(fun_flags.print_instructions){
        print_instructions();
    }

    reg_struct.etp = 0;

    if(NONE == instructions[reg_struct.etp].token_type){
        goto cleanup;
    }

    if(instructions[reg_struct.etp].token_type != TOKEN){
        printf("\e[31mError\e[0m: Invalid token type on line \e[31m%i\e[0m (first token must be instruction)\n", *line_no);
        error_check = -1;
        goto cleanup;
    }

    if(SET == instructions[reg_struct.etp].data.token){
        reg_struct.etp ++;
        if(STRING != instructions[reg_struct.etp].token_type){
            printf("\e[31mError\e[0m: Invalid token type on line \e[31m%i\e[0m (second token of SET instruction must be string)", *line_no);
            error_check = -1;
            goto cleanup;
        }

        error_check = insert_variable(instructions[reg_struct.etp].data.str, 
                                      reg_struct.rtp.reg_64 - (long int)text  /*The offset of where the variable will be appended to the text data section*/, 
                                      false, true, -1, false);
        if(-1 == error_check){
            goto cleanup;
        }

        free(instructions[reg_struct.etp].data.str);

        reg_struct.etp ++;
        error_check = append_variable_to_data(instructions[reg_struct.etp].data.str, instructions[reg_struct.etp].token_type, *line_no);  // THIS MAY BE AN ISSUE: using the str of the data union when it may be num
        if(-1 == error_check){
            goto cleanup;
        }

        free(instructions[reg_struct.etp].data.str);
    }

    else if(TAG == instructions[reg_struct.etp].data.token){
        reg_struct.etp ++;
        if(STRING != instructions[reg_struct.etp].token_type){
            printf("\e[31mError\e[0m: Invalid token type on line \e[31m%i\e[0m (second token of TAG instruction must be string)", *line_no);
            error_check = -1;
            goto cleanup;
        }

        offset = lseek(compiled_fd, 0, SEEK_CUR);
        if(-1 == offset){
            error_check = print_error("\e[31mMANAGE_SEQUENCE\e[0m: Lseek error", -1);
            goto cleanup;
        }

        error_check = insert_variable(instructions[reg_struct.etp].data.str, offset, true, true, -1, false);
        if(-1 == error_check){
            goto cleanup;
        }

        free(instructions[reg_struct.etp].data.str);
    }

    else{
        error_check = write(compiled_fd, (short int *)line_no, sizeof(short int));
        if(-1 == error_check){
            print_error("\e[31mMANAGE_SEQUENCE\e[0m: Write error", -1);
            goto cleanup;
        }

        for(reg_struct.etp=0; reg_struct.etp < INSTRUCTION_SIZE; reg_struct.etp ++){
            if(instructions[reg_struct.etp].token_type != STRING && instructions[reg_struct.etp].token_type != TAGGEE){
                error_check = write(compiled_fd, &instructions[reg_struct.etp].token_type, sizeof(instructions[reg_struct.etp].token_type));
                if(-1 == error_check){
                    print_error("\e[31mMANAGE_SEQUENCE\e[0m: Write error", -1);
                    goto cleanup;
                }
            }
            
            switch(instructions[reg_struct.etp].token_type){
                case TOKEN: error_check = write(compiled_fd, &instructions[reg_struct.etp].data.token, sizeof(instructions[reg_struct.etp].data.token)); break;
                case SIZE: error_check = write(compiled_fd, &instructions[reg_struct.etp].data.size, sizeof(instructions[reg_struct.etp].data.size)); break;
                case REGISTER: error_check = write(compiled_fd, &instructions[reg_struct.etp].data.reg, sizeof(instructions[reg_struct.etp].data.reg)); break;
                case NUM: error_check = write(compiled_fd, &instructions[reg_struct.etp].data.num, sizeof(instructions[reg_struct.etp].data.num)); break;
                case FUNCTION: error_check = write(compiled_fd, &instructions[reg_struct.etp].data.function, sizeof(instructions[reg_struct.etp].data.function)); break;
                case TAGGEE:
                case STRING: {
                    error_check = get_var(instructions[reg_struct.etp].data.str, &var);
                    if(-1 == error_check){      // Variable has not been read yet
                        offset = lseek(compiled_fd, 0, SEEK_CUR);
                        if(-1 == offset){
                            error_check = print_error("\e[31mMANAGE_SEQUENCE\e[0m: Lseek error", -1);
                            goto cleanup;
                        }

                        error_check = insert_variable(instructions[reg_struct.etp].data.str, -1, false, false, offset, true);
                        if(-1 == error_check){
                            goto cleanup;
                        }

                        value = 0;
                        error_check = write(compiled_fd, &value, sizeof(u_int8_t));
                        error_check = write(compiled_fd, &value, sizeof(value));
                    }
                    else{
                        if(var->istag){
                            instructions[reg_struct.etp].token_type = TAGGEE;
                        }

                        error_check = write(compiled_fd, &instructions[reg_struct.etp].token_type, sizeof(instructions[reg_struct.etp].token_type));
                        if(-1 == error_check){
                            print_error("\e[31mMANAGE_SEQUENCE\e[0m: Write error", -1);
                            goto cleanup;
                        }

                        error_check = write(compiled_fd, &var->value, sizeof(var->value));
                    }

                    free(instructions[reg_struct.etp].data.str);

                    break;
                }
                case NONE: goto cleanup;
                default:{
                    printf("\e[31mError\e[0m: Invalid token type value on line \e[31m%i\e[0m", *line_no);
                    error_check = -1;
                    goto cleanup;
                }
            }

            if(-1 == error_check){
                print_error("\e[31mMANAGE_SEQUENCE\e[0m: Write error", -1);
                goto cleanup;
            }
        }
    }

cleanup:
    if(-1 != error_check){
        for(reg_struct.etp++; reg_struct.etp < INSTRUCTION_SIZE; reg_struct.etp++){
            if(instructions[reg_struct.etp].token_type != NONE){
                puts("\e[33mWarning:\e[0m extra token (will be ignored)");
            }
        }
    }

    return error_check;
}

int compile(char * source_name, char * compiled_name, func_flags_t fun_flags){
    int error_check = 0;
    file_t * source = NULL;
    int compiled_fd = -1;
    int temp_fd = -1;
    int i = 0;
    long int j = 0;
    off_t main_offset = 0;
    long int text_size = 0;
    char temp_file_name[FILE_NAME_LEN] = {0};
    char buffer[BUFFER_SIZE] = {0};
    u_int8_t tag_type = 0;
    bool_t eof = false;
    variable_t * curr_var = NULL;

    source = MF_open(source_name);
    if(NULL == source){
        error_check = -1;
        goto cleanup;
    }

    for(j=0; j<=999999999999999; j++){
        sprintf(temp_file_name, "%li", j);
        errno = 0;
        temp_fd = open(temp_file_name, O_RDWR | O_EXCL | O_CREAT, 0666);
        if(-1 == temp_fd){
            if(EEXIST == errno){
                continue;
            }
            else{
                error_check = print_error("\e[31mCOMPILE\e[0m: Open error", -1);
                goto cleanup;
            }
        }
        break;
    }

    while(!MF_eof(source)){
        i ++;
        error_check = manage_sequence(source, temp_fd, &i, fun_flags);
        if(-1 == error_check){
            goto cleanup;
        }
    }

    for(i=0; i<BUFFER_SIZE; i++){
        curr_var = variables[i];
        
        while(curr_var != NULL){
            if(!curr_var->value_set){
                printf("\e[31mError\e[0m: tag/var \e[31m%s\e[0m is referenced but never set.\n", curr_var->name);
                error_check = -1;
                goto cleanup;
            }

            for(j=0; j<curr_var->offsets.length; j++){
                error_check = lseek(temp_fd, curr_var->offsets.list[j], SEEK_SET);
                if(-1 == error_check){
                    print_error("\e[31mCOMPILE\e[0m: Lseek error", -1);
                    goto cleanup;
                }

                if(curr_var->istag){
                    tag_type = TAGGEE;
                }
                else{
                    tag_type = STRING;
                }

                error_check = write(temp_fd, &tag_type, sizeof(tag_type));
                if(-1 == error_check){
                    print_error("\e[31mCOMPILE\e[0m: Write error", -1);
                    goto cleanup;
                }

                error_check = write(temp_fd, &curr_var->value, sizeof(curr_var->value));
                if(-1 == error_check){
                    print_error("\e[31mCOMPILE\e[0m: Write error", -1);
                    goto cleanup;
                }
            }

            curr_var = curr_var->next;
        }
    }

    compiled_fd = open(compiled_name, O_RDWR | O_TRUNC | O_CREAT, 0666);
    if(-1 == compiled_fd){
        error_check = print_error("\e[31mCOMPILE\e[0m: Open error", -1);
        goto cleanup;
    }

    error_check = write(compiled_fd, magic, magic_len+1);
    if(-1 == error_check){
        print_error("\e[31mCOMPILE\e[0m: Write error", -1);
        goto cleanup;
    }
    
    error_check = write(compiled_fd, version, sizeof(version));
    if(-1 == error_check){
        print_error("\e[31mCOMPILE\e[0m: Write error", -1);
        goto cleanup;
    }

    error_check = get_value(ENTRY_POINT, &main_offset);
    if(-1 == error_check){
        printf("\e[31mError\e[0m: No %s tag\n", ENTRY_POINT);
        goto cleanup;
    }
    error_check = write(compiled_fd, &main_offset, sizeof(main_offset));
    if(-1 == error_check){
        print_error("\e[31mCOMPILE\e[0m: Write error", -1);
        goto cleanup;
    }
    free_variables();
    
    text_size = reg_struct.rtp.reg_64 - (long int)text;
    error_check = write(compiled_fd, &text_size, sizeof(text_size));
    if(-1 == error_check){
        print_error("\e[31mCOMPILE\e[0m: Write error", -1);
        goto cleanup;
    }
    error_check = write(compiled_fd, text, text_size);
    if(-1 == error_check){
        print_error("\e[31mCOMPILE\e[0m: Write error", -1);
        goto cleanup;
    }

    error_check = lseek(temp_fd, 0, SEEK_SET);
    if(-1 == error_check){
        error_check = print_error("\e[31mCOMPILE\e[0m: Lseek error", -1);
        goto cleanup;
    }

    while(!eof){
        i = read(temp_fd, buffer, sizeof(buffer));
        if(-1 == i){
            error_check = print_error("\e[31mCOMPILE\e[0m: Read error", -1);
            goto cleanup;
        }

        error_check = write(compiled_fd, buffer, i);
        if(-1 == error_check){
            print_error("\e[31mCOMPILE\e[0m: Read error", -1);
            goto cleanup;
        }

        if(i < sizeof(buffer)){
            eof = true;
        }
    }

    munmap(text, getpagesize());

cleanup:
    if(0 != temp_file_name[0]){
        remove(temp_file_name);
    }
    if(NULL != source){
        MF_close(source);
    }
    if(-1 != compiled_fd){
        close(compiled_fd);
    }

    return error_check;
}
