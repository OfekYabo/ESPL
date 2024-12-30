CC = gcc
AS = nasm

# Compiler and assembler flags
CFLAGS = -m32 -Wall -g
ASFLAGS = -f elf32

# Source files
C_SOURCES = main.c util.c
ASM_SOURCES = start.s

# Object files
C_OBJECTS = $(C_SOURCES:.c=.o)
ASM_OBJECTS = $(ASM_SOURCES:.s=.o)

# Executable name
EXEC = program

# Default target
all: $(EXEC)

# Link the executable
$(EXEC): $(C_OBJECTS) $(ASM_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile C source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble assembly source files
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

# Clean up
clean:
	rm -f $(C_OBJECTS) $(ASM_OBJECTS) $(EXEC)

.PHONY: all clean




# all: start main util link

# link: start.o util.o main.o
# 	ld -m elf_i386 start.o util.o main.o -o task2
	
# start: start.s
# 	nasm -f elf32 start.s -o start.o

# util: util.c
# 	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o

# main: main.c
# 	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector main.c -o main.o	

# clean:
# 	rm -f *.o all start task2