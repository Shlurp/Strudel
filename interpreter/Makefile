PROJECT_NAME = ../stru
CC = gcc
SRC_DIR = ./src
INCLUDE_DIR = ../include ./include
OBJ_DIR = ./obj

CFLAGS = -Wall $(patsubst %,-I%,$(INCLUDE_DIR))

DEPS = $(wildcard $(INCLUDE_DIR)/*.h)
_OBJ = $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(_OBJ))

all: $(OBJ_DIR) $(PROJECT_NAME)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) -c -g $(CFLAGS) $< -o $@

$(PROJECT_NAME): $(OBJ)
	$(CC) -g $(CFLAGS) $^ -o $@

clean:
	rm $(OBJ_DIR)/*.o

.PHONY: all clean