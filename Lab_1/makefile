#helped by copilot

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -m32 -Wall -Wextra -std=c99

# Target executable
TARGET = menu

# Source files
SRCS = menu_map.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Link the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(TARGET) $(OBJS)

# Phony targets
.PHONY: all clean