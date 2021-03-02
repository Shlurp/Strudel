COMP_DIR = compiler
INTER_DIR = interpreter

all: compiler interpreter

compiler:
	make -C $(COMP_DIR)

interpreter:
	make -C $(INTER_DIR)

clean:
	rm -r $(COMP_DIR)/obj/*.o
	rm -r $(INTER_DIR)/obj/*.o

.PHONY: all compiler interpreter