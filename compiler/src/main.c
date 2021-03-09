#include "comp.h"

int main(int argc, char ** argv){
    char * dest = "stru.out";
    int error_check = 0;
    int i = 0;
    int j = 0;
    int dest_len = 0;
    list_t input_files = {0};
    char * obj_files[BUFFER_SIZE] = {0};
    bool_t dest_raised = false;
    func_flags_t fun_flags = {0};

    if(argc < 2){
        printf("\e[31mUsage\e[0m: $ %s <source> ...\n", argv[0]);
        goto cleanup;
    }

    error_check = init_list(&input_files);
    if(-1 == error_check){
        goto cleanup;
    }

    for(i=1; i<argc; i++){
        j = 0;
        if('-' == argv[i][0]){
            for(j=1; argv[i][j] != 0 && !dest_raised; j++){
                switch(argv[i][j]){
                    case 'i': fun_flags.print_instructions = 1; break;
                    case 'h': print_help(); goto cleanup;
                    case 'o': dest_raised = true; break;
                    case 'c': fun_flags.no_link = 1; break;
                    case 'l': fun_flags.only_link = 1; break;
                    default: printf("\e[31mError\e[0m: Invalid flag (\e[31m-%c\e[0m)\n", argv[i][j]); goto cleanup;
                }
            }
        }
        else if(!dest_raised){
            error_check = append_element(&input_files, (long int)(argv[i]));
            if(-1 == error_check){
                goto cleanup;
            }
        }
        if(dest_raised && argv[i][j] != 0){
            dest_len = strnlen(argv[i]+j, BUFFER_SIZE);
            dest = calloc(dest_len + 1, sizeof(char));
            memcpy(dest, argv[i]+j, dest_len);
            dest_raised = false;
        }
    }

    if(fun_flags.no_link && fun_flags.only_link){
        puts("\e[31mError\e[0m: Cannot raise -c (no-link) and -l (only-link) flags together");
        goto cleanup;
    }

    init();

    if(fun_flags.only_link){
        error_check = linker(dest, input_files.length, (char **)input_files.list);
        if(-1 == error_check){
            goto cleanup;
        }
    }
    else{
        for(i=0; i<input_files.length; i++){
            error_check = replace_extension((char *)input_files.list[i], "o", &(obj_files[i]));
            if(-1 == error_check){
                goto cleanup;
            }
        }

        if(!fun_flags.no_link){
            for(i=0; i<input_files.length; i++){
                error_check = compile((char *)input_files.list[i], obj_files[i], fun_flags);
                if(-1 == error_check){
                    goto cleanup;
                }

                reg_struct.rtp.reg_64 = (long int)text;
                memset(text, 0, page_size);
            }
            
            linker(dest, input_files.length, obj_files);

            for(i=0; i<input_files.length; i++){
                remove(obj_files[i]);
                free(obj_files[i]);
            }
        }
        else{
            if(input_files.length != 1){
                puts("\e[31mError\e[0m: Must input \e[1mone\e[0m input file");
                goto cleanup;
            }

            error_check = compile((char *)input_files.list[0], dest, fun_flags);
            if(-1 == error_check){
                goto cleanup;
            }
        }
    }

    free_list(&input_files);

cleanup:
    exit(0);
}
