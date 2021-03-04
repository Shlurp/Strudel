#ifndef COMP_DATATYPES_H
#define COMP_DATATYPES_H

#define LIST_CHUNK_SIZE 16

typedef struct file_s{
    char * start;
    long int length;
    long int offset;
    long int map_size;
}file_t;

typedef struct list_s{
    long int * list;
    int length;         // Length of list
    int num_chunks;     // Number of chunks allocated
}list_t;

typedef struct variable_s{
    struct variable_s * next;

    char * name;            // Variable name
    bool_t value_set;       // If the value is set
    long int value;         // Value of the variable
    bool_t istag;           // True if it's a tag, false for string
    list_t offsets;         // Offsets in the file to insert the justified value at
}variable_t;

#endif
