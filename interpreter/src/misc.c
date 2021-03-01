#include "inter.h"

flags_t flags = {0};
bool_t newline = false;
int * stack = NULL;
char * text = NULL;
registers_t reg_struct = {0};
instruction_t instructions[INSTRUCTION_SIZE] = {0};
jump_offset_t * jump_offsets[BUFFER_SIZE] = {0};
var_t * text_vars[BUFFER_SIZE] = {0};
off_t code_start = 0;
char * magic = "DUMDUM";
int magic_len = 0;
int version[3] = {0, 0, 0};
int oldest_compatible[3] = {0, 0, 0};

int init(){
    int return_value = 0;

    magic_len = strnlen(magic, BUFFER_SIZE);

    stack = mmap(NULL, getpagesize(), PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if(MAP_FAILED == stack){
        return_value = print_error("\e[31mINIT: mmap error\e[0m", -1);
        goto cleanup;
    }

    reg_struct.rsp.reg_64 = (long int)stack;
    reg_struct.rbp.reg_64 = (long int)stack;

cleanup:
    return return_value;
}

void print_help(){
    char * prompt = "\n"
    "\e[32;1mDumDum\e[0m - an educational runtime interpreter with no real goal.\n\n"
    "\e[1mUsage\e[0m:\n"
    "\t\e[1mdum\e[0m [\e[4mOPTION\e[0m]... <\e[4mFILE\e[0m>\n"
    "Interprets FILE\n\n"
    "\e[1mFlags\e[0m\n"
    "\t\e[1m-s\e[0m\tShows the stack after each sequence (after execution)\n"
    "\t\e[1m-r\e[0m\tShows the state of the registers after each sequence (after execution)\n"
    "\t\e[1m-i\e[0m\tShows the parsed instructions after each sequence (before execution)\n"
    "\t\e[1m-f\e[0m\tShows the state of the flags after each sequence (after execution)\n"
    "\t\e[1m-h\e[0m\tShows this text\n";

    puts(prompt);
}

#if 0
int insert_var(char * name, void * addr){
    int return_value = 0;
    int name_len = 0;
    int diff = 0;
    var_t * node = NULL;
    var_t * next = NULL;
    int index = 0;

    index = (unsigned int)hash((unsigned char *)name) % BUFFER_SIZE;

    next = text_vars[index];
    while(NULL != next){
        diff = strncmp(next->name, name, BUFFER_SIZE);
        if(0 == diff){
            next->addr = addr;
            goto cleanup;
        }

        next = next->next;
    }

    node = malloc(sizeof(var_t));
    if(NULL == node){
        return_value = print_error("\e[31mINSERT_VAR\e[0m: Malloc error", -1);
        goto cleanup;
    }
    name_len = strnlen(name, BUFFER_SIZE);
    node->name = malloc(name_len + 1);
    if(NULL == node->name){
        return_value = print_error("\e[31mINSERT_VAR\e[0m: Malloc error", -1);
        goto cleanup;
    }
    memcpy(node->name, name, name_len+1);
    node->addr = addr;
    node->next = text_vars[index];

    text_vars[index] = node;

cleanup:
    return return_value;
}

int set_text_var(){
    int return_value = 0;
    int str_ptr = 0;
    int buff_ptr = 0;
    void * addr = 0;
    char * name = NULL;
    char * value = NULL;
    char buffer[BUFFER_SIZE] = {0};

    if(instructions[reg_struct.etp].token_type != STRING){
        puts("\e[31mError\e[0m: expected string for var name\n");
        return_value = -1;
        goto cleanup;
    }

    name = instructions[reg_struct.etp].data.str;
    addr = (void *)reg_struct.rtp.reg_64;

    reg_struct.etp ++;

    if(NUM == instructions[reg_struct.etp].token_type){
        *(long int *)reg_struct.rtp.reg_64 = instructions[reg_struct.etp].data.num;
        reg_struct.rtp.reg_64 += sizeof(long int);
    }

    else if(STRING == instructions[reg_struct.etp].token_type){
        value = instructions[reg_struct.etp].data.str;
        if('"' != value[str_ptr]){
            puts("\e[31mError\e[0m: expected \" at beginning of value\n");
            return_value = -1;
            goto cleanup;
        }

        str_ptr ++;

        while(0 != value[str_ptr] && '"' != value[str_ptr]){
            //printf("%c\n", value[str_ptr]);
            if('\\' == value[str_ptr]){
                str_ptr ++;

                switch(value[str_ptr]){
                    case 'n': buffer[buff_ptr] = '\n'; break;
                    case 'r': buffer[buff_ptr] = '\r'; break;
                    case 'b': buffer[buff_ptr] = '\b'; break;
                    case 'e': buffer[buff_ptr] = '\e'; break;
                    case '\\': buffer[buff_ptr] = '\\'; break;
                    default:
                        printf("\e[31mError\e[0m: \e[31m\\%c\e[0m is an unrecognized escape character\n", value[str_ptr]);
                        return_value = -1;
                        goto cleanup;
                }
            }
            else{
                buffer[buff_ptr] = value[str_ptr];
            }

            buff_ptr ++;

            str_ptr ++;
        }

        if('"' != value[str_ptr]){
            printf("\e[31mError\e[0m: expected \" at end of value, got \e[31m%c\e[0m instead\n", value[str_ptr-1]);
            return_value = -1;
            goto cleanup;
        }

        memcpy((void *)reg_struct.rtp.reg_64, buffer, buff_ptr + 1);
        
        reg_struct.rtp.reg_64 += buff_ptr + 1;
    }

    return_value = insert_var(name, addr);
    free(name);
    instructions[reg_struct.etp-1].token_type = NONE;

cleanup:
    return return_value;
}

void * get_var_value(char * name){
    void * addr = NULL;
    int diff = 0;
    int index = 0;
    var_t * node = NULL;

    index = (unsigned int)hash((unsigned char *)name) % BUFFER_SIZE;

    node = text_vars[index];

    while(node != NULL){
        diff = strncmp(node->name, name, BUFFER_SIZE);
        if(0 == diff){
            break;
        }

        node = node->next;
    }

    if(NULL == node){
        addr = FAIL;
        goto cleanup;
    }

    addr = node->addr;

cleanup:
    return addr;
}
#endif