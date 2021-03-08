#include "comp.h"

int get_temp_file(char temp_file_name[FILE_NAME_LEN]){
    long int j = 0;
    int temp_fd = -1;

    for(j=0; j<=999999999999999; j++){
        sprintf(temp_file_name, "%li", j);
        errno = 0;
        temp_fd = open(temp_file_name, O_RDWR | O_EXCL | O_CREAT, 0666);
        if(-1 == temp_fd){
            if(EEXIST == errno){
                continue;
            }
            else{
                print_error("\e[31mCOMPILE\e[0m: Open error", -1);
                goto cleanup;
            }
        }
        break;
    }

cleanup:
    return temp_fd;
}

int replace_extension(char * filename, char * new_extension, char ** new_filename){
    int error_check = 0;
    int extension_index = 0;
    int new_extension_len = 0;
    int filename_len = 0;

    filename_len = strnlen(filename, BUFFER_SIZE);

    for(extension_index=filename_len-1; extension_index > 0 && filename[extension_index] != '.'; extension_index--);
    if(0 == extension_index){
        extension_index = filename_len;
    }

    new_extension_len = strnlen(new_extension, BUFFER_SIZE);

    *new_filename = malloc(extension_index + new_extension_len + 2);
    if(NULL == new_filename){
        error_check = print_error("\e[31mREPLACE_EXTENSION\e[0m: Malloc error", -1);
        goto cleanup;
    }

    memcpy(*new_filename, filename, extension_index+1);
    memcpy(*new_filename + extension_index + 1, new_extension, new_extension_len+1);

cleanup:
    return error_check;
}
