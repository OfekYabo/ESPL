# helped by copilot
# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c99

# Executable names
TARGET_SHELL = myshell
TARGET_PIPE = mypipeline

# Source files
SRCS_SHELL = myshell.c LineParser.c
SRCS_PIPE = mypipeline.c

# Object files
OBJS_SHELL = $(SRCS_SHELL:.c=.o)
OBJS_PIPE = $(SRCS_PIPE:.c=.o)

# Output files
ERROR_SHELL_LOG = ErrorShell.log
ERROR_PIPE_LOG = ErrorPipeLine.log

# Default target
all: $(TARGET_SHELL) $(TARGET_PIPE)

# Compile myshell
$(TARGET_SHELL): $(OBJS_SHELL)
	$(CC) $(CFLAGS) -o $(TARGET_SHELL) $(OBJS_SHELL)

# Compile mypipeline
$(TARGET_PIPE): $(OBJS_PIPE)
	$(CC) $(CFLAGS) -o $(TARGET_PIPE) $(OBJS_PIPE)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run myshell with arguments and redirect error output
run_myshell: $(TARGET_SHELL)
	./$(TARGET_SHELL) 2> $(ERROR_SHELL_LOG)

run_myshell_d: $(TARGET_SHELL)
	./$(TARGET_SHELL) -d 2> $(ERROR_SHELL_LOG)

# Run mypipeline and redirect error output
run_mypipeline: $(TARGET_PIPE)
	./$(TARGET_PIPE) 2> $(ERROR_PIPE_LOG)

# Clean up object files and executable
clean:
	rm -f $(OBJS_SHELL) $(TARGET_SHELL) $(ERROR_SHELL_LOG) $(OBJS_PIPE) $(TARGET_PIPE) $(ERROR_PIPE_LOG)
# Phony targets
.PHONY: all myshell run_myshell run_mypipeline clean