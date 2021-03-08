#include "comp.h"

variable_t * variables[BUFFER_SIZE] = {0};

int insert_variable(char * variable_name, long int value, bool_t istag, bool_t isglobal, bool_t insert_value, off_t offset, bool_t insert_offset){
    int return_value = 0;
    unsigned int name_hash = 0;
    int diff = 0;
    int name_len = 0;
    variable_t * curr_node = NULL;

    name_hash = (unsigned int)(hash((unsigned char *)variable_name) % BUFFER_SIZE);
    curr_node = variables[name_hash];
    
    while(curr_node != NULL){
        diff = strncmp(curr_node->name, variable_name, BUFFER_SIZE);
        if(0 == diff){
            break;
        }

        curr_node = curr_node->next;
    }

    if(NULL == curr_node){
        curr_node = malloc(sizeof(variable_t));
        if(NULL == curr_node){
            return_value = print_error("\e[31mINSERT_VARIABLE_OFFSET\e[0m: Malloc error", -1);
            goto cleanup;
        }

        name_len = strnlen(variable_name, BUFFER_SIZE);
        curr_node->name = calloc(name_len + 1, sizeof(char));
        if(NULL == curr_node->name){
            return_value = print_error("\e[31mINSERT_VARIABLE_OFFSET\e[0m: Calloc error", -1);
            goto cleanup;
        }
        memcpy(curr_node->name, variable_name, name_len);

        return_value = init_list(&curr_node->offsets);
        if(-1 == return_value){
            goto cleanup;
        }
        if(insert_offset){
            return_value = append_element(&curr_node->offsets, offset);
            if(-1 == return_value){
                goto cleanup;
            }
        }

        if(insert_value){
            curr_node->value_set = true;
            curr_node->value = value;
            curr_node->istag = istag;
            curr_node->isglobal = isglobal;
        }
        else{
            curr_node->isglobal = false;
            curr_node->istag = false;
            curr_node->value_set = false;
            curr_node->value = 0;
        }

        curr_node->next = variables[name_hash];
        variables[name_hash] = curr_node;
    }
    else{
        if(insert_offset){
            return_value = append_element(&curr_node->offsets, offset);
            if(-1 == return_value){
                goto cleanup;
            }
        }

        if(insert_value){
            curr_node->value_set = true;
            curr_node->value = value;
            curr_node->istag = istag;
            curr_node->isglobal = isglobal;
        }
        #if 0
        else{       // This should make the program fail, no?
            curr_node->value_set = false;
        }
        #endif
    }

cleanup:
    return return_value;
}

void print_variables(bool_t new_line){
    int i = 0;
    variable_t * curr_var = NULL;

    putchar('<');
    for(i=0; i<BUFFER_SIZE; i++){
        curr_var = variables[i];

        while(curr_var != NULL){
            if(curr_var->value_set){
                if(curr_var->istag){
                    printf("#%s: {%li, ", curr_var->name, curr_var->value);
                }
                else{
                    printf("%s: {%li, ", curr_var->name, curr_var->value);
                }
            }
            else{
                printf("%s: {~, ", curr_var->name);
            }
            print_list(&curr_var->offsets, false);
            fputs("}, ", stdout);

            curr_var = curr_var->next;
        }
    }
    fputs("\e[2D", stdout);
    putchar('>');

    if(new_line){
        putchar('\n');
    }
}

void free_variables(){
    int i = 0;
    variable_t * curr_node = NULL;
    variable_t * next_node = NULL;

    for(i=0; i<BUFFER_SIZE; i++){
        curr_node = variables[i];

        while(curr_node != NULL){
            next_node = curr_node->next;
            free_list(&curr_node->offsets);
            if(curr_node->name != NULL){
                free(curr_node->name);
            }
            free(curr_node);
            curr_node = next_node;
        }
    }

    memset(variables, 0, sizeof(variables));
}

int get_value(char * variable_name, long int * value){
    int return_value = -1;
    int diff = 0;
    unsigned int name_hash = 0;
    variable_t * var = NULL;

    name_hash = (unsigned int)(hash((unsigned char *)variable_name) % BUFFER_SIZE);

    var = variables[name_hash];
    while(var != NULL){
        diff = strncmp(variable_name, var->name, BUFFER_SIZE);
        if(0 == diff){
            if(var->value_set){
                return_value = 0;
                *value = var->value;
            }
            break;
        }

        var = var->next;
    }

    return return_value;
}

int get_var(char * var_name, variable_t ** var){
    int return_value = -1;
    int diff = 0;
    unsigned int name_hash = 0;

    name_hash = (unsigned int)(hash((unsigned char *)var_name) % BUFFER_SIZE);

    *var = variables[name_hash];
    while(*var != NULL){
        diff = strncmp(var_name, (*var)->name, BUFFER_SIZE);
        if(0 == diff){
            if((*var)->value_set){
                return_value = 0;
            }
            break;
        }

        *var = (*var)->next;
    }

    return return_value;
}

int append_variable_to_data(char * value, token_type_t size, int line_no){
    int return_value = 0;
    char buffer[BUFFER_SIZE] = {0};
    char character = 0;
    int buffer_ptr = 0;
    int str_ptr = 0;

    if(STRING == size){
        if(value[str_ptr] == '"'){
            str_ptr ++;
            while(value[str_ptr] != 0 && value[str_ptr] != '"'){
                if('\\' == value[str_ptr]){
                    str_ptr ++;

                    switch(value[str_ptr]){
                        case 'n': buffer[buffer_ptr] = '\n'; break;
                        case 'r': buffer[buffer_ptr] = '\r'; break;
                        case 'b': buffer[buffer_ptr] = '\b'; break;
                        case 'e': buffer[buffer_ptr] = '\e'; break;
                        case '\\': buffer[buffer_ptr] = '\\'; break;
                        case '"': buffer[buffer_ptr] = '"'; break;
                        default:{
                            printf("\e[31mError\e[0m on line \e[31m%i\e[0m: unsupported escape code \e[31m\\%c\e[0m\n", line_no, value[str_ptr]);
                            return_value = -1;
                            goto cleanup;
                        }
                    }
                }
                else{
                    buffer[buffer_ptr] = value[str_ptr];
                }


                buffer_ptr ++;
                str_ptr ++;
            }

            if(value[str_ptr] != '"'){
                printf("\e[31mError\e[0m on line \e[31m%i\e[0m: string expected to end with \", got \e[31m%c\e[0m instead\n", line_no, value[str_ptr]);
                return_value = -1;
                goto cleanup;
            }

            memcpy((void *)reg_struct.rtp.reg_64, buffer, buffer_ptr + 1);
            reg_struct.rtp.reg_64 += buffer_ptr + 1;
        }
        else if(value[str_ptr] == '\''){
            str_ptr ++;

            if('\\' == value[str_ptr]){
                str_ptr ++;

                switch(value[str_ptr]){
                    case 'n': character = '\n'; break;
                    case 'r': character = '\r'; break;
                    case 'b': character = '\b'; break;
                    case 'e': character = '\e'; break;
                    case '\\': character = '\\'; break;
                    case '\'': character = '\''; break;
                    default:{
                        printf("\e[31mError\e[0m on line \e[31m%i\e[0m: unsupported escape code \e[31m\\%c\e[0m\n", line_no, value[str_ptr]);
                        return_value = -1;
                        goto cleanup;
                    }
                }
            }
            else{
                character = value[str_ptr];
            }

            str_ptr ++;

            if(value[str_ptr] != '\''){
                printf("\e[31mError\e[0m on line \e[31m%i\e[0m: character expected to end with ', got \e[31m%c\e[0m instead\n", line_no, value[str_ptr]);
                return_value = -1;
                goto cleanup;
            }

            *(char *)reg_struct.rtp.reg_64 = character;
            reg_struct.rtp.reg_64 ++;
        }
        else{
            printf("\e[31mError\e[0m on line \e[31m%i\e[0m: string expected to start with \" or ', got \e[31m%c\e[0m instead\n", line_no, value[str_ptr]);
            return_value = -1;
            goto cleanup;
        }
    }
    else if(NUM == size){
        *(long int *)reg_struct.rtp.reg_64 = (long int)value;
        reg_struct.rtp.reg_64 += sizeof(long int);
    }

    else{
        printf("\e[31mError\e[0m on line \e[31m%i\e[0m: invalid datatype encountered while appending value to text data\n", line_no);
        return_value = -1;
        goto cleanup;
    }
    
cleanup:
    return return_value;
}

int write_vars(int fd){
    int error_check = 0;
    int name_len = 0;
    int i = 0;
    int num_vars = 0;
    off_t start_off = 0;
    off_t end_off = 0;
    variable_t * curr_var = NULL;

    start_off = lseek(fd, 0, SEEK_CUR);
    if(-1 == start_off){
        print_error("\e[31mWRITE_VARS\e[0m: Lseek error", -1);
        goto cleanup;
    }

    error_check = write(fd, &num_vars, sizeof(num_vars));
    if(-1 == error_check){
        print_error("\e[31mWRITE_VARS\e[0m: Write error", -1);
        goto cleanup;
    }

    for(i=0; i<BUFFER_SIZE; i++){
        curr_var = variables[i];
        while(curr_var != NULL){
            name_len = strnlen(curr_var->name, BUFFER_SIZE);
            error_check = write(fd, &name_len, sizeof(name_len));
            if(-1 == error_check){
                print_error("\e[31mWRITE_VARS\e[0m: Write error", -1);
                goto cleanup;
            }

            error_check = write(fd, curr_var->name, name_len);
            if(-1 == error_check){
                print_error("\e[31mWRITE_VARS\e[0m: Write error", -1);
                goto cleanup;
            }

            error_check = write(fd, &curr_var->value_set, sizeof(curr_var->value_set));
            if(-1 == error_check){
                print_error("\e[31mWRITE_VARS\e[0m: Write error", -1);
                goto cleanup;
            }

            if(curr_var->value_set){
                error_check = write(fd, &curr_var->value, sizeof(curr_var->value));
                if(-1 == error_check){
                    print_error("\e[31mWRITE_VARS\e[0m: Write error", -1);
                    goto cleanup;
                }
            }

            error_check = write(fd, &curr_var->istag, sizeof(curr_var->istag));
            if(-1 == error_check){
                print_error("\e[31mWRITE_VARS\e[0m: Write error", -1);
                goto cleanup;
            }

            error_check = write(fd, &curr_var->isglobal, sizeof(curr_var->isglobal));
            if(-1 == error_check){
                print_error("\e[31mWRITE_VARS\e[0m: Write error", -1);
                goto cleanup;
            }

            error_check = write_list(fd, &curr_var->offsets);
            if(-1 == error_check){
                goto cleanup;
            }

            num_vars ++;
            curr_var = curr_var->next;
        }
    }

    end_off = lseek(fd, 0, SEEK_CUR);
    if(-1 == end_off){
        print_error("\e[31mWRITE_VARS\e[0m: Lseek error", -1);
        goto cleanup;
    }

    error_check = lseek(fd, start_off, SEEK_SET);
    if(-1 == error_check){
        print_error("\e[31mWRITE_VARS\e[0m: Lseek error", -1);
        goto cleanup;
    }

    error_check = write(fd, &num_vars, sizeof(num_vars));
    if(-1 == error_check){
        print_error("\e[31mWRITE_VARS\e[0m: Write error", -1);
        goto cleanup;
    }

    error_check = lseek(fd, end_off, SEEK_SET);
    if(-1 == error_check){
        print_error("\e[31mWRITE_VARS\e[0m: Lseek error", -1);
        goto cleanup;
    }


cleanup:
    return error_check;
}

int read_var(int fd, variable_t * var){
    int error_check = 0;
    int name_len = 0;

    error_check = read(fd, &name_len, sizeof(name_len));
    if(-1 == error_check){
        print_error("\e[31mREAD_VAR\e[0m: Read error", -1);
        goto cleanup;
    }

    var->name = calloc(name_len+1, sizeof(char));
    if(NULL == var->name){
        print_error("\e[31mREAD_VAR\e[0m: Calloc error", -1);
        goto cleanup;
    }

    error_check = read(fd, var->name, name_len);
    if(-1 == error_check){
        print_error("\e[31mREAD_VAR\e[0m: Read error", -1);
        goto cleanup;
    }

    error_check = read(fd, &var->value_set, sizeof(var->value_set));
    if(-1 == error_check){
        print_error("\e[31mREAD_VAR\e[0m: Read error", -1);
        goto cleanup;
    }

    if(var->value_set){
        error_check = read(fd, &var->value, sizeof(var->value));
        if(-1 == error_check){
            print_error("\e[31mREAD_VAR\e[0m: Read error", -1);
            goto cleanup;
        }
    }

    error_check = read(fd, &var->istag, sizeof(var->istag));
    if(-1 == error_check){
        print_error("\e[31mREAD_VAR\e[0m: Read error", -1);
        goto cleanup;
    }

    error_check = read(fd, &var->isglobal, sizeof(var->isglobal));
    if(-1 == error_check){
        print_error("\e[31mREAD_VAR\e[0m: Read error", -1);
        goto cleanup;
    }

    error_check = read_list(fd, &var->offsets);
    if(-1 == error_check){
        goto cleanup;
    }

cleanup:
    return error_check;
}
