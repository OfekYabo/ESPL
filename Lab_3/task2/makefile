CC = gcc
AS = nasm
LD = ld

# Compiler and assembler flags
CFLAGS = -m32 -Wall -g -fno-stack-protector
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386

# Source files
C_SOURCES = main.c util.c
ASM_SOURCES = start.s

# Object files
C_OBJECTS = $(C_SOURCES:.c=.o)
ASM_OBJECTS = $(ASM_SOURCES:.s=.o)
OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

# Executable name
EXEC = task2

# Default target
all: $(EXEC)

# Link the executable using ld
$(EXEC): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

# Compile C source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble assembly source files
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

# Clean up
clean:
	rm -f $(OBJECTS) $(EXEC)

.PHONY: all clean