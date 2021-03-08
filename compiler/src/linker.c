#include "comp.h"

int linker(char * output, int num_files, char ** file_names){
    int error_check = 0;
    int output_fd = 0;
    int temp_fd = 0;
    int code_len = 0;
    long int data_len = 0;
    long int curr_data_len = 0;
    int curr_data_map_len = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    int diff = 0;
    int num_of_vars = 0;
    u_int8_t tag_type = 0;
    off_t curr_code_start = 0;
    off_t curr_off = 0;
    off_t entry_off = 0;
    int version_check[3] = {0};
    char temp_file_name[FILE_NAME_LEN] = {0};
    char buffer[BUFFER_SIZE] = {0};
    char * magic_check = NULL;
    char * curr_data = NULL;
    int curr_fd = 0;
    variable_t curr_file_var = {0};
    variable_t * temp_var = NULL;

    temp_fd = get_temp_file(temp_file_name);
    if(-1 == temp_fd){
        error_check = -1;
        goto cleanup;
    }

    magic_check = malloc(obj_magic_len + 1);
    if(NULL == magic_check){
        error_check = print_error("\e[31mLINKER\e[0m: Malloc error", -1);
        goto cleanup;
    }

    for(i=0; i<num_files; i++){
        curr_fd = open((const char *)file_names[i], O_RDWR);
        if(-1 == curr_fd){
            error_check = print_error("\e[31mLINKER\e[0m: Fopen error", -1);
            goto cleanup;
        }

        error_check = read(curr_fd, magic_check, obj_magic_len+1);
        if(-1 == error_check){
            print_error("\e[31mLINKER\e[0m: Fread error", -1);
            goto cleanup;
        }

        diff = strncmp(magic_check, obj_magic, obj_magic_len+1);
        if(diff != 0){
            puts("\e[33mLinker\e[0m: \e[31mError\e[0m: Invalid object file");
            error_check = -1;
            goto cleanup;
        }

        error_check = read(curr_fd, version, sizeof(version));
        if(-1 == error_check){
            print_error("\e[31mLINKER\e[0m: Fread error", -1);
            goto cleanup;
        }

        if(version_check[0] < oldest_compatible[0]){
            puts("\e[31mError\e[0m: Compiled file is incompatible with current version.");
            error_check = -1;
            goto cleanup;
        }
        else if(version_check[0] == oldest_compatible[0]){
            if(version_check[1] < oldest_compatible[1]){
                puts("\e[31mError\e[0m: Compiled file is incompatible with current version.");
                error_check = -1;
                goto cleanup;
            }
            else if(version_check[1] == oldest_compatible[1]){
                if(version_check[2] < oldest_compatible[2]){
                    puts("\e[31mError\e[0m: Compiled file is incompatible with current version.");
                    error_check = -1;
                    goto cleanup;
                }
            }
        }

        error_check = read(curr_fd, &curr_code_start, sizeof(curr_code_start));
        if(-1 == error_check){
            print_error("\e[31mLINKER\e[0m: Fread error", -1);
            goto cleanup;
        }

        error_check = read(curr_fd, &num_of_vars, sizeof(num_of_vars));
        if(-1 == error_check){
            print_error("\e[31mLINKER\e[0m: Fread error", -1);
            goto cleanup;
        }

        // Offsets are wrong
        for(j=0; j<num_of_vars; j++){
            error_check = read_var(curr_fd, &curr_file_var);
            if(-1 == error_check){
                goto cleanup;
            }

            if(curr_file_var.istag && curr_file_var.value_set){
                if(curr_file_var.istag){
                    curr_file_var.value += code_len;
                }
                else{
                    curr_file_var.value += data_len;
                }
            }

            if(curr_file_var.isglobal || !curr_file_var.istag || !curr_file_var.value_set){
                error_check = get_var(curr_file_var.name, &temp_var);
                if(-1 == error_check){
                    error_check = insert_variable(curr_file_var.name, curr_file_var.value, curr_file_var.istag, curr_file_var.isglobal, curr_file_var.value_set, 0, false);
                    if(-1 == error_check){
                        goto cleanup;
                    }

                    error_check = get_var(curr_file_var.name, &temp_var);
                }
                else{
                    if(temp_var->value_set && curr_file_var.value_set){
                        printf("\e[31mError\e[0m: \e[31m%s\e[0m set in two files\n", curr_file_var.name);
                        error_check = -1;
                        goto cleanup;
                    }
                    if(temp_var->istag != curr_file_var.istag){
                        printf("\e[31mError\e[0m: \e[31m%s\e[0m defined as two different types\n", curr_file_var.name);
                        error_check = -1;
                        goto cleanup;
                    }

                    if(curr_file_var.value_set){
                        temp_var->value = curr_file_var.value;
                        temp_var->value_set = curr_file_var.value_set;
                    }
                }

                for(k=0; k<curr_file_var.offsets.length; k++){
                    curr_file_var.offsets.list[k] += code_len;
                }

                error_check = append_list(&temp_var->offsets, &curr_file_var.offsets);
                if(-1 == error_check){
                    goto cleanup;
                }
            }

            else{
                curr_off = lseek(curr_fd, 0, SEEK_CUR);
                if(-1 == curr_off){
                    error_check = print_error("\e[31mLINKER\e[0m: Lseek error", -1);
                    goto cleanup;
                }
                
                for(k=0; k<curr_file_var.offsets.length; k++){
                    error_check = lseek(curr_fd, curr_file_var.offsets.list[k]+curr_code_start, SEEK_SET);
                    if(-1 == error_check){
                        print_error("\e[31mLINKER\e[0m: Lseek error", -1);
                        goto cleanup;
                    }

                    if(curr_file_var.istag){
                        tag_type = TAGGEE;
                    }
                    else{
                        tag_type = STRING;
                    }

                    error_check = write(curr_fd, &tag_type, sizeof(tag_type));
                    if(-1 == error_check){
                        print_error("\e[31mLINKER\e[0m: Write error", -1);
                        goto cleanup;
                    }

                    error_check = write(curr_fd, &curr_file_var.value, sizeof(curr_file_var.value));
                    if(-1 == error_check){
                        print_error("\e[31mLINKER\e[0m: Write error", -1);
                        goto cleanup;
                    }
                }

                error_check = lseek(curr_fd, curr_off, SEEK_SET);
                if(-1 == error_check){
                    print_error("\e[31mLINKER\e[0m: Lseek error", -1);
                    goto cleanup;
                }
            }

            free(curr_file_var.name);
            free_list(&curr_file_var.offsets);
        }

        error_check = read(curr_fd, &curr_data_len, sizeof(curr_data_len));
        if(-1 == error_check){
            print_error("\e[31mLINKER\e[0m: Read error", -1);
            goto cleanup;
        }

        if(curr_data_len % page_size){
            curr_data_map_len = curr_data_len;
        }
        else{
            curr_data_map_len = (curr_data_len / page_size + 1) * page_size;
        }

        curr_data = mmap(NULL, curr_data_map_len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if(MAP_FAILED == curr_data){
            print_error("\e[31mLINKER\e[0m: Mmap error", -1);
            goto cleanup;
        }

        error_check = read(curr_fd, curr_data, curr_data_len);
        if(-1 == error_check){
            print_error("\e[31mLINKER\e[0m: Read error", -1);
            goto cleanup;
        }
        memcpy(text + data_len, curr_data, curr_data_len);
        data_len += curr_data_len;

        error_check = munmap(curr_data, curr_data_map_len);
        if(MAP_FAILED == curr_data){
            print_error("\e[31mLINKER\e[0m: Munmap error", -1);
            goto cleanup;
        }

        while(true){
            error_check = read(curr_fd, buffer, BUFFER_SIZE);
            if(-1 == error_check){
                print_error("\e[31mLINKER\e[0m: Read error", -1);
                goto cleanup;
            }
            else if(0 == error_check){
                break;
            }

            error_check = write(temp_fd, buffer, error_check);
            if(-1 == error_check){
                print_error("\e[31mLINKER\e[0m: Write error", -1);
                goto cleanup;
            }
            code_len += error_check;
        }
    }

    for(i=0; i<BUFFER_SIZE; i++){
        temp_var = variables[i];

        while(temp_var != NULL){
            if(!temp_var->value_set){
                printf("\e[31mError\e[0m: Variable \e[31m%s\e[0m referenced but never declared\n", temp_var->name);
                error_check = -1;
                goto cleanup;
            }

            for(j=0; j<temp_var->offsets.length; j++){
                error_check = lseek(temp_fd, temp_var->offsets.list[j], SEEK_SET);
                if(-1 == error_check){
                    print_error("\e[31mLINKER\e[0m: Lseek error", -1);
                    goto cleanup;
                }

                if(temp_var->istag){
                    tag_type = TAGGEE;
                }
                else{
                    tag_type = STRING;
                }

                error_check = write(temp_fd, &tag_type, sizeof(tag_type));
                if(-1 == error_check){
                    print_error("\e[31mLINKER\e[0m: Write error", -1);
                    goto cleanup;
                }

                error_check = write(temp_fd, &temp_var->value, sizeof(temp_var->value));
                if(-1 == error_check){
                    print_error("\e[31mLINKER\e[0m: Write error", -1);
                    goto cleanup;
                }
            }

            temp_var = temp_var->next;
        }
    }

    output_fd = open(output, O_RDWR | O_TRUNC | O_CREAT, 0666);
    if(-1 == output_fd){
        error_check = print_error("\e[31mLINKER\e[0m: Open error", -1);
        goto cleanup;
    }

    error_check = write(output_fd, magic, magic_len+1);
    if(-1 == error_check){
        print_error("\e[31mLINKER\e[0m: Write error", -1);
        goto cleanup;
    }

    error_check = write(output_fd, version, sizeof(version));
    if(-1 == error_check){
        print_error("\e[31mLINKER\e[0m: Write error", -1);
        goto cleanup;
    }

    error_check = get_value(ENTRY_POINT, &entry_off);
    if(-1 == error_check){
        printf("\e[31mError\e[0m: No entry (%s) tag\n", ENTRY_POINT);
        goto cleanup;
    }

    error_check = write(output_fd, &entry_off, sizeof(entry_off));
    if(-1 == error_check){
        print_error("\e[31mLINKER\e[0m: Write error", -1);
        goto cleanup;
    }

    error_check = write(output_fd, &data_len, sizeof(data_len));
    if(-1 == error_check){
        print_error("\e[31mLINKER\e[0m: Write error", -1);
        goto cleanup;
    }

    error_check = write(output_fd, text, data_len);
    if(-1 == error_check){
        print_error("\e[31mLINKER\e[0m: Write error", -1);
        goto cleanup;
    }

    error_check = lseek(temp_fd, 0, SEEK_SET);
    if(-1 == error_check){
        print_error("\e[31mLINKER\e[0m: Lseek error", -1);
        goto cleanup;
    }

    while(true){
        error_check = read(temp_fd, buffer, BUFFER_SIZE);
        if(-1 == error_check){
            print_error("\e[31mLINKER\e[0m: Read error", -1);
            goto cleanup;
        }
        else if(0 == error_check){
            break;
        }

        error_check = write(output_fd, buffer, error_check);
        if(-1 == error_check){
            print_error("\e[31mLINKER\e[0m: Write error", -1);
            goto cleanup;
        }
    }

cleanup:
    if(magic_check != NULL){
        free(magic_check);
    }

    if(0 != temp_file_name[0]){
        remove(temp_file_name);
    }

    free_variables();

    return error_check;
}
