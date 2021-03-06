#include "./data_types.h"
#include "global_libs.h"

#define MF_ungetc(file) (file->offset --)
#define MF_eof(file) ((file->offset > file->length) ? 1 : 0)

void print_help();

unsigned long hash(unsigned char * str);

file_t * MF_open(char * path);

int get_temp_file(char temp_file_name[FILE_NAME_LEN]);

int init_list(list_t * list);
int insert_element(list_t * list, long int element, int index);
int set_element(list_t * list, long int element, int index);
int append_element(list_t * list, long int element);
int pop_element(list_t * list, int index);
void print_list(list_t * list, bool_t new_line);
int write_list(int fd, list_t * list);
int read_list(int fd, list_t * list);
int append_list(list_t * dest, list_t * src);

int insert_variable(char * variable_name, long int value, bool_t istag, bool_t isglobal, bool_t insert_value, off_t offset, bool_t insert_offset);
void print_variables(bool_t new_line);
int get_value(char * variable_name, long int * value);
int get_var(char * var_name, variable_t ** var);
void free_variables();
int append_variable_to_data(char * value, token_type_t size, int line_no);
int write_vars(int fd);
int read_var(int fd, variable_t * var);

int init();

int replace_extension(char * filename, char * new_extension, char ** new_filename);

int compile(char * source_name, char * compiled_name, func_flags_t fun_flags);
int linker(char * output, int num_files, char ** file_names);

void print_instructions();

static inline void MF_read(file_t * file, char * buffer, int size){
    memcpy(buffer, (char *)(file->start + file->offset), size);
    file->offset += size;
}

static inline char MF_getc(file_t * file){
    file->offset ++;
    return *(char *)(file->start + file->offset - 1);
}

static inline void MF_close(file_t * file){
    munmap(file->start, file->map_size);
    free(file);
}

static inline void free_list(list_t * list){
    free(list->list);
    list->list = NULL;
}
