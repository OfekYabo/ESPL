# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c99

# Executable name
TARGET_SHELL = myshell
TARGET_PIPE = mypipe

# Source files
SRCS_SHELL = myshell.c LineParser.c
SRCS_PIPE = mypipe.c

# Object files
OBJS_SHELL = $(SRCS_SHELL:.c=.o)
OBJS_PIPE = $(SRCS_PIPE:.c=.o)

# Output file
ERROR_SHELL_LOG = ErrorShell.log
ERROR_PIPE_LOG = ErrorPipe.log

# Default target
all: $(TARGET_SHELL) $(TARGET_PIPE)

# Compile myshell
$(TARGET_SHELL): $(OBJS_SHELL)
	$(CC) $(CFLAGS) -o $(TARGET_SHELL) $(OBJS_SHELL)

# Compile mypipe
$(TARGET_PIPE): $(OBJS_PIPE)
	$(CC) $(CFLAGS) -o $(TARGET_PIPE) $(OBJS_PIPE)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run myshell with arguments and redirect error output
run_myshell: $(TARGET_SHELL)
	./$(TARGET_SHELL) $(ARGS) 2> $(ERROR_LOG)

# Run mypipe and redirect error output
run_mypipe: $(TARGET_PIPE)
	./$(TARGET_PIPE) 2> $(ERROR_PIPE_LOG)

# Clean up object files and executable
clean:
	rm -f $(OBJS_SHELL) $(TARGET_SHELL) $(ERROR_LOG)

# Phony targets
.PHONY: all myshell run clean