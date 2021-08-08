CC = gcc
OBJ_DIR = ./obj
DSA_SRC_DIR = ./src/dynamic_string_array
DS_SRC_DIR = ./src/dynamic_string
INC = -I./include

# the object files sans directory path
_OBJS = dsa_add_remove.o dsa_core.o dsa_err_handling.o dsa_hash.o dsa_modify.o \
	dsa_multiset_ops.o dsa_set_ops.o dsa_sort.o dsa_strings.o dsa_tests.o \
	ds_str_funcs.o

# the objects files with directory path
OBJS = $(patsubst %.o, $(OBJ_DIR)/%.o, $(_OBJS))

# make the run_tests binary and the static library dystring.a
all: run_tests libdystring.a

# compile all the src files into the binary
run_tests: $(DSA_SRC_DIR)/*.c $(DS_SRC_DIR)/*.c ./include/*.h
	$(CC) -o run_tests $(INC) $(DSA_SRC_DIR)/*.c $(DS_SRC_DIR)/*.c

# make the object files from the dynamic_string directory
$(OBJ_DIR)/%.o: $(DS_SRC_DIR)/%.c ./include/*.h
	$(CC) -c $(INC) -o $@ $< $(CFLAGS)

# make the object files from the dynamic_string_array directory
$(OBJ_DIR)/%.o: $(DSA_SRC_DIR)/%.c $(DS_SRC_DIR)/*.c ./include/*.h
	$(CC) -c $(INC) -o $@ $< $(CFLAGS)

# make the library
libdystring.a: $(OBJS)
	ar rcs libdystring.a $(OBJS)

# remove object libraries, object files, and binaries
clean:
	rm -f ./*.a $(OBJ_DIR)/*.o ./run_tests