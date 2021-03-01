#ifndef CONSTS_H
#define CONSTS_H

#define DEBUG_STR "\e[31mDEBUG\e[0m"
#define DEBUG (puts(DEBUG_STR));

#define STACK_SIZE (2048)
#define TEXT_SIZE (1024)
#define BUFFER_SIZE (512)
#define INSTRUCTION_SIZE (16)
#define NUM_REG_SIZE (10)
#define STACK_ELEMENT_SIZE (8)

#define R_REG_SIZE (64)
#define E_REG_SIZE (32)
#define X_REG_SIZE (16)
#define HL_REG_SIZE (8)

#define ENTRY_POINT "MAIN"

#define true (1)
#define false (0)

#define NONE (0)

#define FAIL ((void *)(-1))

#define ABS(num) ((num < 0) ? -1 * num : num)

#endif
