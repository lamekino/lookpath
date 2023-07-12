PROG_NAME = lookpath
SRC_DIR = ./src
TEST_SRC = ./test

BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj

TEST_OBJ = $(BUILD_DIR)/test_obj
TEST_EXE = $(BUILD_DIR)/test_exe

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

CC = gcc
INCLUDE = -I$(SRC_DIR)

ifeq ($(DEBUG),1)
	CCFLAGS = \
		-std=c9x -ggdb -Wall -Wextra -Werror -Wno-unused-variable \
		-Wno-unused-parameter -Wno-unused-but-set-variable \
		-Wno-unused-function -Wno-unused-local-typedefs \
		-D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=500 -DDEBUG=1
else
	CCFLAGS = \
		-std=c9x -Wall -Wextra -Werror -Wno-unused-local-typedefs -O3 \
		-D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=500
endif

SRC = $(shell find $(SRC_DIR) -name "*.c")
TESTS = $(shell find $(TEST_SRC) -name "*.c")

OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
OBJ_TESTS = $(patsubst $(TEST_SRC)/%.c, $(TEST_OBJ)/%.o, $(TESTS))

all: $(PROG_NAME)

clean:
	rm -fr $(BUILD_DIR)

install: all
	install -m 0755 $(BUILD_DIR)/$(PROG_NAME) $(PREFIX)/bin

compile_commands.json: clean
	@bear -- make

$(PROG_NAME): $(OBJ)
	$(CC) -o $(BUILD_DIR)/$@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@) || true
	$(CC) $(INCLUDE) $(CCFLAGS) -c -o $@ $<

$(TEST_OBJ)/%.o: $(TEST_SRC)/%.c
	@mkdir -p $(dir $@) || true
	$(CC) -lcheck $(INCLUDE) $(CCFLAGS) -c -o $@ $<

test: $(OBJ_TESTS) all
	@mkdir -p $(TEST_OBJ) || true
	@mkdir -p $(TEST_EXE) || true
	for obj in $(OBJ_TESTS); do \
		OUT=$$(basename -s .o $$obj); \
		$(CC) -l check -o $(TEST_EXE)/$$OUT $$obj \
			$$(find $(OBJ_DIR) -mindepth 1 -not -name main.o); \
	done; \
	for exe in $(TEST_EXE)/*; do \
		printf '=>> Running test %s\n' "$$exe"; \
		$$exe; \
	done

.PHONY: all clean install test compile_commands.json
