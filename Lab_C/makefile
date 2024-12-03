# # Compiler
# CC = gcc

# # Compiler flags
# CFLAGS = -Wall -Wextra -std=c99

# # Executable name
# TARGET_SHELL = myshell

# # Source files
# SRCS_SHELL = myshell.c LineParser.c

# # Object files
# OBJS_SHELL = myshell.o LineParser.o

# # Output file
# ERROR_LOG = ErrorShell.log

# # Default target
# all: $(TARGET_SHELL)

# # Compile myshell.o
# myshell.o: myshell.c
# 	$(CC) $(CFLAGS) -c myshell.c -o myshell.o

# # Compile LineParser.o
# LineParser.o: LineParser.c
# 	$(CC) $(CFLAGS) -c LineParser.c -o LineParser.o

# # Link the object files to create the executable
# $(TARGET_SHELL): $(OBJS_SHELL)
# 	$(CC) $(CFLAGS) -o $(TARGET_SHELL) $(OBJS_SHELL)

# # Run myshell with arguments and redirect error output
# run: $(TARGET_SHELL)
# 	./$(TARGET_SHELL) $(ARGS) 2> $(ERROR_LOG)

# # Clean up object files and executable
# clean:
# 	rm -f $(OBJS_SHELL) $(TARGET_SHELL) $(ERROR_LOG)

# # Phony targets
# .PHONY: all run clean


# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c99

# Executable name
TARGET_SHELL = myshell

# Source files
SRCS_SHELL = myshell.c LineParser.c

# Object files
OBJS_SHELL = $(SRCS_SHELL:.c=.o)

# Output file
ERROR_LOG = ErrorShell.log

# Default target
all: $(TARGET_SHELL)

# Compile myshell
myshell: $(OBJS_SHELL)
	$(CC) $(CFLAGS) -o $(TARGET_SHELL) $(OBJS_SHELL)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run myshell with arguments and redirect error output
run: myshell
	./$(TARGET_SHELL) $(ARGS) 2> $(ERROR_LOG)

# Clean up object files and executable
clean:
	rm -f $(OBJS_SHELL) $(TARGET_SHELL) $(ERROR_LOG)

# Phony targets
.PHONY: all myshell run clean