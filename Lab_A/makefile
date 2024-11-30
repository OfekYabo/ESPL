# Compiler
CC = gcc

# Compiler flags
CFLAGS = -m32 -Wall -Wextra -std=c99

# Target executable
TARGET = encoder

# Source files
SRCS = encoder.c

# Object files
OBJS = $(SRCS:.c=.o)

# Log file for stderr
LOGFILE = error.log

# Default target
all: $(TARGET)

# Link the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile C source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program with arguments and redirect stderr to a log file
run: $(TARGET)
	./$(TARGET) $(ARGS) 2> $(LOGFILE)

# Clean up build files and log file
clean:
	rm -f $(TARGET) $(OBJS) $(LOGFILE)

# Phony targets
.PHONY: all clean run