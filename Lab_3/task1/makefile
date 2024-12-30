# Makefile for compiling the assembly code

AS = nasm
LD = ld
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386

all: encoder

encoder: start.o util.o
	$(LD) $(LDFLAGS) -o encoder start.o util.o

start.o: start.s
	$(AS) $(ASFLAGS) -o start.o start.s

util.o: util.c
	gcc -m32 -c -o util.o util.c

clean:
	rm -f *.o encoder