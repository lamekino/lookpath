PROG_NAME = lookpath
OBJ_DIR = ./obj
SRC_DIR = ./src

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

CC = gcc
INCLUDE = -I$(SRC_DIR)

ifeq ($(DEBUG),1)
	CCFLAGS = \
		-std=c9x -ggdb -Wall -Wextra -Werror -Wno-unused-variable \
		-Wno-unused-parameter -Wno-unused-but-set-variable -Wno-unused-function\
		-D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=500 -DDEBUG=1
else
	CCFLAGS = \
		-std=c9x -Wall -Wextra -Werror -O3 -D_POSIX_C_SOURCE=200809L \
		-D_XOPEN_SOURCE=500
endif

SRC = $(shell find $(SRC_DIR) -name "*.c")
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

all: $(PROG_NAME)

clean:
	rm -fr $(OBJ_DIR) *$(PROG_NAME)

install: all
	install -m 0755 $(PROG_NAME) $(PREFIX)/bin

compile_commands.json: clean
	@bear -- make

$(PROG_NAME): $(OBJ)
	$(CC) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@) || true
	$(CC) $(INCLUDE) $(CCFLAGS) -c -o $@ $<

.PHONY: all clean install compile_commands.json
