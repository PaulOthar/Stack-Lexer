include makes/util.mk

SRC_DIR = ./src
INC_DIR = ./include
BIN_DIR = ./bin

SRC = $(call rwildcard,$(SRC_DIR),*.c)
SRC += ./test/*.c
OUTPUT = $(BIN_DIR)/bin

CFLAGS = -Wall -Wextra -O0 -g3

CC = gcc

include makes/release.mk
all:
	@$(call build_dependency,./lib)
	$(CC) $(SRC) -o $(OUTPUT) -I$(INC_DIR) $(CFLAGS)

include makes/valgrind.mk
mcall:
	$(call valprof,$(OUTPUT))
	$(call valk)

LIBNAME = dislexer

.PHONY: release
release:
	$(call build_release,$(LIBNAME),$(LIBS))