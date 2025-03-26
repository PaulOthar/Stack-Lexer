SRC_DIR = ./src
INC_DIR = ./include
BIN_DIR = ./bin

SRC = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*/*.c)
OUTPUT = $(BIN_DIR)/bin.exe

CFLAGS = -Wall -Wextra -O0 -g3 -std=c99

CC = gcc

all:
	$(CC) $(SRC) -o $(OUTPUT) -I$(INC_DIR) $(CFLAGS)