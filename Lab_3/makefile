# Define the compiler, assembler and linker
CC = gcc
ASM = nasm
LD = ld

# Define the compiler, assembler and linker flags
CFLAGS = -m32 -Wall -ansi -c -nostdlib -fno-stack-protector
ASMFLAGS = -f elf32
LDFLAGS = -m elf_i386

# Define the source and object files
SRC_C = util.c main.c
SRC_ASM = start.s
OBJ_C = $(SRC_C:.c=.o)
OBJ_ASM = $(SRC_ASM:.s=.o)
TARGET = task0

# Default target: build the executable
all: $(TARGET)

# Rule to build the target executable
$(TARGET): $(OBJ_ASM) $(OBJ_C)
	$(LD) $(LDFLAGS) $(OBJ_ASM) $(OBJ_C) -o $(TARGET)

# Rule to assemble the assembly source files
%.o: %.s
	$(ASM) $(ASMFLAGS) $< -o $@

# Rule to compile the C source files
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

# Clean up the build files
clean:
	rm -f $(OBJ_ASM) $(OBJ_C) $(TARGET)