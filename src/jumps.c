#include "inter.h"

/**
 * djb2 hash algorithm by Dan Bernstein: http://www.cse.yorku.ca/~oz/hash.html
 */
unsigned long hash(unsigned char * str){
    unsigned long hash = 5381;
    int c = 0;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

int insert_jump_offset(char * str, off_t offset){
    int return_value = 0;
    int index = 0;
    int diff = 0;
    jump_offset_t * node = NULL;
    jump_offset_t * next = NULL;
    jump_offset_t * curr_node = NULL;

    index = ABS((int)hash(str)) % BUFFER_SIZE;

    node = malloc(sizeof(jump_offset_t));
    if(NULL == node){
        return_value = print_error("\e[31mINSERT_JUMP_OFFSET\e[0m: Malloc error", -1);
        goto cleanup;
    }

    node->tag = calloc(strnlen(str, BUFFER_SIZE) + 1, sizeof(char));
    memcpy(node->tag, str, strnlen(str, BUFFER_SIZE));
    node->offset = offset;

    next = jump_offsets[index];
    
    while(curr_node != NULL){
        diff = strncmp(next->tag, str, BUFFER_SIZE);
        if(0 == diff){
            next->offset = offset;
            goto cleanup;
        }
    }

    next = jump_offsets[index];
    node->next = next;

    jump_offsets[index] = node;
    

cleanup:
    return return_value;
}

off_t get_jump_offset(char * str){
    off_t offset = 0;
    int index = 0;
    int i = 0;
    int diff = 0;
    jump_offset_t * node = NULL;

    index = ABS((int)hash(str)) % BUFFER_SIZE;

    node = jump_offsets[index];

    for(i=0; i<BUFFER_SIZE && node != NULL; i++){
        diff = strncmp(node->tag, str, BUFFER_SIZE);
        if(0 == diff){
            break;
        }

        node = node->next;
    }

    if(NULL == node){
        offset = -1;
    }
    else{
        offset = node->offset;
    }

    return offset;
}

off_t find_tag(FILE * source, char * str){
    off_t origin_offset = 0;
    off_t curr_offset = 0;
    off_t tag_offset = -1;
    int diff = 0;
    int error_check = 0;
    char curr_char = 0;
    instruction_t instruction = {0};
    jump_offset_t * node = NULL;

    origin_offset = ftell(source);
    if(-1 == origin_offset){
        tag_offset = print_error("\e[31mFIND_TAG\e[0m: Ftell error", -1);
        goto cleanup;
    }

    do{
        newline = false;
        error_check = get_next_instruction(source, &instruction);
        if(-1 == error_check){
            goto cleanup;
        }

        if(TOKEN == instruction.token_type && TAG == instruction.data.token){
            error_check = get_next_instruction(source, &instruction);
            if(-1 == error_check){
                goto cleanup;
            }

            curr_char = 0;
            
            while('\n' != curr_char && !newline){
                error_check = fread(&curr_char, sizeof(curr_char), 1, source);
                if(0 == error_check){
                    if(feof(source)){
                        printf("\e[31mError\e[0m on line \e[31m%i\e[0m: Tag can't be on eof\n", get_curr_line(source));
                        tag_offset = -1;
                        goto cleanup;   
                    }
                    else{
                        tag_offset = print_error("\e[31mFIND_TAG\e[0m: fread error", -1);
                        goto cleanup;
                    }
                }
            }

            newline = false;

            if(STRING != instruction.token_type){
                printf("\e[31mError\e[0m on line \e[31m%i\e[0m: Expected string\n", get_curr_line(source));
                goto cleanup;
            }

            curr_offset = ftell(source);
            if(-1 == curr_offset){
                tag_offset = print_error("\e[31mFIND_TAG\e[0m: Ftell error", -1);
                goto cleanup;
            }

            error_check = insert_jump_offset(instruction.data.str, curr_offset);
            if(-1 == error_check){
                goto cleanup;
            }

            diff = strncmp(instruction.data.str, str, BUFFER_SIZE);
            free(instruction.data.str);
        }
        else{
            diff = -1;
        }
    }while(diff != 0 && !feof(source));

    origin_offset = rseek(source, origin_offset, SEEK_SET);
    if(-1 == origin_offset){
        goto cleanup;
    }

    tag_offset = get_jump_offset(str);
    if(-1 == tag_offset){
        printf("\e[31mError\e[0m: Tag \e[31m%s\e[0m doesn't exist\n", str);
        goto cleanup;
    }

cleanup:
    return tag_offset;
}

int jump(FILE * source){
    int return_value = 0;
    long int temp = 0;

    if(STRING != instructions[reg_struct.etp].token_type){
        printf("\e[31mError\e[0m on line \e[31m%i\e[0m: expected string\n", get_curr_line(source));
        return_value = -1;
        goto cleanup;
    }

    temp = get_jump_offset(instructions[reg_struct.etp].data.str);
    if(-1 == temp){
        temp = find_tag(source, instructions[reg_struct.etp].data.str);
        if(-1 == temp){
            return_value = -1;
            goto cleanup;
        }
    }

    return_value = rseek(source, temp, SEEK_SET);
    if(-1 == return_value){
        return_value = print_error("\e[31mEXECUTE_INSTRUCTIONS\e[0m: Fseek error", -1);
        goto cleanup;
    }

cleanup:
    return return_value;
}
