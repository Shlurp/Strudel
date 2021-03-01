#include "comp.h"

file_t * MF_open(char * path){
    int fd = 0;
    int page_size = 0;
    off_t file_size = 0;
    size_t map_size = 0;
    file_t * file = NULL;

    file = malloc(sizeof(file_t));
    if(NULL == file){
        file = ptr_print_error("\e[31mINIT_FILE\e[0m: Malloc error", NULL);
        goto cleanup;
    }

    fd = open(path, O_RDONLY);
    if(-1 == fd){
        file = ptr_print_error("\e[31mINIT_FILE\e[0m: Open error", NULL);
        goto cleanup;
    }

    file_size = lseek(fd, 0, SEEK_END);
    if(-1 == file_size){
        file = ptr_print_error("\e[31mINIT_FILE\e[0m: Lseek error", NULL);
        goto cleanup;
    }

    page_size = getpagesize();

    if(file_size % page_size != 0){
        map_size = (file_size / page_size + 1) * page_size;
    }
    else{
        map_size = file_size;
    }

    file->start = mmap(NULL, map_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(MAP_FAILED == file->start){
        file = ptr_print_error("\e[31mINIT_FILE\e[0m: Mmap error", NULL);
        goto cleanup;
    }

    file->offset = 0;
    file->length = file_size;
    file->map_size = map_size;

cleanup:
    return file;
}

