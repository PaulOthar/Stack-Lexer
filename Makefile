SRC_DIR = ./src
INC_DIR = ./include
BIN_DIR = ./bin
REL_DIR = ./release

SRC = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*/*.c)
OUTPUT = $(BIN_DIR)/bin.exe

CFLAGS = -Wall -Wextra -O0 -g3 -std=c99

CC = gcc

all:
	$(CC) $(SRC) -o $(OUTPUT) -I$(INC_DIR) $(CFLAGS)

#-------------------------------------------------
	
LIBNAME = stack_lexer
OS := $(shell uname)
ifeq ($(OS), Linux)
	OSTYPE = linux
else
	OSTYPE = windows
endif

RELEASE = $(REL_DIR)/$(LIBNAME)_$(OSTYPE).o
RELEASE_ASM = $(REL_DIR)/$(LIBNAME)_$(OSTYPE).a
	
launch:
	$(CC) -c $(SRC_DIR)/$(LIBNAME).c -o $(RELEASE) -I$(INC_DIR)
	ar rcs $(RELEASE_ASM) $(RELEASE)