CC = gcc
OBJ_DIR = ./obj
MAIN_SRC = ./src
MAIN_INC = ./include

# this is extremely crappy
HASH_SRC = ./dependencies/generic_hash_dict
LEV_SRC = ./dependencies/Levenshteiner
SHARED_SRC = ./dependencies/shared
UNI_SRC = ./dependencies/unicode_helper

INC = -I./$(MAIN_INC) -I./$(HASH_SRC) -I./$(LEV_SRC) -I./$(SHARED_SRC) -I./$(UNI_SRC)

# the object files sans directory path
_OBJS = game_dict_lang_handling.o game_dict.o game_menu.o \
	game_round.o game_status.o validating_dict_files.o

# the objects files with directory path
OBJS = $(patsubst %.o, $(OBJ_DIR)/%.o, $(_OBJS))

# make the run_tests binary and the static library dystring.a
all: dystrings entropic_hangman

# make the dystrings library
dystrings:
	cd ./dependencies/dynamic_strings && $(MAKE) all

# compile all the src files into the binary
entropic_hangman: $(MAIN_SRC)/*.c $(HASH_SRC)/*.c $(LEV_SRC)/*.c $(SHARED_SRC)/*.c $(UNI_SRC)/*.c ./include/*.h
	$(CC) -o entropic_hangman $(INC) -I./dependencies/dynamic_strings $(MAIN_SRC)/*.c $(HASH_SRC)/*.c $(LEV_SRC)/*.c $(SHARED_SRC)/*.c $(UNI_SRC)/*.c -L./dependencies/dynamic_strings -ldystring -lsodium

# remove object libraries, object files, and binaries
clean:
	rm -f ./*.a $(OBJ_DIR)/*.o ./entropic_hangman && cd ./dependencies/dynamic_strings && $(MAKE) clean