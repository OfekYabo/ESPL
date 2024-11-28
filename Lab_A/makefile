# Compiler
CC = gcc

# Assembler
AS = nasm

# Compiler flags
CFLAGS = -m32 -Wall -Wextra -std=c99

# Assembler flags
ASFLAGS = -f elf

# Target executable
TARGET = main

# Source files
SRCS = main.c numbers.c

# Assembly files
ASM_SRCS = ass.s

# Object files
OBJS = $(SRCS:.c=.o) $(ASM_SRCS:.s=.o)

# Default target
all: $(TARGET)

# Link the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile C source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble assembly source files into object files
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

# Clean up build files
clean:
	rm -f $(TARGET) $(OBJS)

# Phony targets
.PHONY: all clean