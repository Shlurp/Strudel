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