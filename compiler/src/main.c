#include "comp.h"

int main(int argc, char ** argv){
    char * dest = "a.out";
    int i = 0;
    int j = 0;
    int src_index = 0;
    int dest_len = 0;
    bool_t dest_raised = false;
    func_flags_t fun_flags = {0};

    if(argc < 2){
        printf("\e[31mUsage\e[0m: $ %s <source> ...\n", argv[0]);
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
                    default: printf("\e[31mError\e[0m: Invalid flag (\e[31m-%c\e[0m)\n", argv[i][j]); goto cleanup;
                }
            }
        }
        else if(!dest_raised){
            src_index = i;
        }
        if(dest_raised && argv[i][j] != 0){
            dest_len = strnlen(argv[i]+j, BUFFER_SIZE);
            dest = calloc(dest_len + 1, sizeof(char));
            memcpy(dest, argv[i]+j, dest_len);
            dest_raised = false;
        }
    }

    init();
    compile(argv[src_index], dest, fun_flags);

cleanup:
    exit(0);
}