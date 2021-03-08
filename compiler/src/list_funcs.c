#include "comp.h"

int init_list(list_t * list){
    int return_value = 0;

    list->length = 0;
    list->num_chunks = 1;

    list->list = calloc(list->num_chunks * LIST_CHUNK_SIZE, sizeof(long int));
    if(NULL == list->list){
        return_value = print_error("\e[31mINIT_LIST\e[0m: Calloc error", -1);
        goto cleanup;
    }

cleanup:
    return return_value;
}

int set_element(list_t * list, long int element, int index){
    int return_value = 0;

    if(index > list->num_chunks * LIST_CHUNK_SIZE){
        list->list = realloc(list->list, (list->num_chunks + 1) * LIST_CHUNK_SIZE * sizeof(list->list[0]));
        if(NULL == list->list){
            return_value = print_error("\e[31mINSERT_ELEMENT\e[0m: Realloc error", -1);
            goto cleanup;
        }
        memset(list->list + list->num_chunks * LIST_CHUNK_SIZE, 0, LIST_CHUNK_SIZE * sizeof(list->list[0]));

        list->num_chunks ++;
        list->length ++;
    }

    list->list[index] = element;

cleanup:
    return return_value;
}

int append_element(list_t * list, long int element){
    int return_value = 0;

    if(list->length >= list->num_chunks * LIST_CHUNK_SIZE){
        list->list = realloc(list->list, (list->num_chunks + 1) * LIST_CHUNK_SIZE * sizeof(list->list[0]));
        if(NULL == list->list){
            return_value = print_error("\e[31mINSERT_ELEMENT\e[0m: Realloc error", -1);
            goto cleanup;
        }
        memset(list->list + list->num_chunks * LIST_CHUNK_SIZE, 0, LIST_CHUNK_SIZE * sizeof(list->list[0]));

        list->num_chunks ++;
    }

    list->list[list->length] = element;
    list->length ++;

cleanup:
    return return_value;
}

int insert_element(list_t * list, long int element, int index){
    int return_value = 0;
    void * error_check = NULL;

    printf("Index: %i; Length: %i\n", index, list->length);
    if(index > list->length){
        return_value = 1;
        goto cleanup;
    }

    if(list->length >= list->num_chunks * LIST_CHUNK_SIZE || index > list->num_chunks * LIST_CHUNK_SIZE){
        list->list = realloc(list->list, (list->num_chunks + 1) * LIST_CHUNK_SIZE * sizeof(list->list[0]));
        if(NULL == list->list){
            return_value = print_error("\e[31mINSERT_ELEMENT\e[0m: Realloc error", -1);
            goto cleanup;
        }
        memset(list->list + list->num_chunks * LIST_CHUNK_SIZE, 0, LIST_CHUNK_SIZE * sizeof(list->list[0]));

        list->num_chunks ++;
    }

    error_check = memmove(list->list + index + 1, list->list + index, (list->length - index) * sizeof(list->list[0]));
    if(NULL == error_check){
        return_value = print_error("\e[31mPOP_ELEMENT\e[0m: Memmove error", -1);
        goto cleanup;
    }

    list->list[index] = element;
    list->length ++;

cleanup:
    return return_value;
}

int pop_element(list_t * list, int index){
    int return_value = 0;
    void * error_check = 0;

    if(index >= list->length){
        return_value = 1;
        goto cleanup;
    }

    error_check = memmove(list->list + index, list->list + index + 1, (list->length - index - 1) * sizeof(list->list[0]));
    if(NULL == error_check){
        return_value = print_error("\e[31mPOP_ELEMENT\e[0m: Memmove error", -1);
        goto cleanup;
    }

    list->length --;

cleanup:
    return return_value;
}

void print_list(list_t * list, bool_t new_line){
    int i = 0;

    printf("%i{", list->length);
    for(i=0; i<list->length-1; i++){
        printf("%li, ", list->list[i]);
    }
    if(list->length > 0){
        printf("%li}", list->list[i]);
    }
    else{
        putchar('}');
    }

    if(new_line){
        putchar('\n');
    }
}

int write_list(int fd, list_t * list){
    int error_check = 0;

    error_check = write(fd, &list->length, sizeof(list->length));
    if(-1 == error_check){
        print_error("\e[31mWRITE_LIST\e[0m: Write error", -1);
        goto cleanup;
    }

    error_check = write(fd, list->list, sizeof(long int) * list->length);
    if(-1 == error_check){
        print_error("\e[31mWRITE_LIST\e[0m: Write error", -1);
        goto cleanup;
    }

    error_check = 0;

cleanup:
    return error_check;
}

int read_list(int fd, list_t * list){
    int error_check = 0;

    error_check = read(fd, &list->length, sizeof(list->length));
    if(-1 == error_check){
        print_error("\e[31mREAD_LIST\e[0m: Read error", -1);
        goto cleanup;
    }

    if(list->length % LIST_CHUNK_SIZE == 0){
        list->num_chunks = list->length;
    }
    else{
        list->num_chunks = (list->length / LIST_CHUNK_SIZE + 1) * LIST_CHUNK_SIZE;
    }

    list->list = calloc(list->num_chunks, sizeof(long int));
    if(NULL == list->list){
        error_check = print_error("\e[31mREAD_LIST\e[0m: Calloc error", -1);
        goto cleanup;
    }

    error_check = read(fd, list->list, list->length * sizeof(long int));
    if(-1 == error_check){
        print_error("\e[31mREAD_LIST\e[0m: Read error", -1);
        goto cleanup;
    }

cleanup:
    return error_check;
}

int append_list(list_t * dest, list_t * src){
    int error_check = 0;
    int temp_len = dest->length;

    dest->length += src->length;
    if(dest->length % LIST_CHUNK_SIZE == 0){
        dest->num_chunks = dest->length;
    }
    else{
        dest->num_chunks = (dest->length / LIST_CHUNK_SIZE + 1) * LIST_CHUNK_SIZE;
    }

    dest->list = realloc(dest->list, dest->num_chunks * sizeof(long int));
    if(NULL == dest->list && dest->num_chunks != 0){
        error_check = print_error("\e[31mAPPEND_LIST\e[0m: Realloc error", -1);
        goto cleanup;
    }

    memcpy(dest->list + temp_len, src->list, src->length * sizeof(long int));

cleanup:
    return error_check;
}
