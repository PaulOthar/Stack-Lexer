SRC_DIR = ./src
INC_DIR = ./include
BIN_DIR = ./bin
REL_DIR = ./release

SRC = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*/*.c)
OUTPUT = $(BIN_DIR)/bin.exe
RELEASE = $(REL_DIR)/stack_lexer.o
ASM = $(REL_DIR)/libstack_lexer.a

CFLAGS = -Wall -Wextra -O0 -g3 -std=c99

CC = gcc

all:
	$(CC) $(SRC) -o $(OUTPUT) -I$(INC_DIR) $(CFLAGS)
	
publish:
	$(CC) -c $(SRC_DIR)/stack_lexer.c -o $(RELEASE) -I$(INC_DIR)
	ar rcs $(ASM) $(RELEASE)