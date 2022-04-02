# this makefile is a stripped down version of this one:
# https://makefiletutorial.com/#makefile-cookbook

TARGET_EXEC := game

CC := gcc

BUILD_DIR := ./build
SRC_DIRS := ./src

CFLAGS := -O3
LINKERFLAGS := -lm -lSDL2 -lSDL2_image -lSDL2_ttf

# Find all the C files we want to compile
# Note the single quotes around the * expressions. Make will incorrectly expand these otherwise.
SRCS := $(shell find $(SRC_DIRS) -name '*.c')

# String substitution for every C file.
# As an example, hello.cpp turns into ./build/hello.cpp.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# The final build step.
$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) $(LINKERFLAGS) -o game

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LINKERFLAGS) -c $< -o $@


.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
	rm game
