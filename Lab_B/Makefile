CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = AntiVirus
SRC = AntiVirus.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) -iinstructions.txt -oprogramOutput.txt

clean:
	cp infected infected-copy
	rm -f $(OBJ) $(TARGET) programOutput.txt

.PHONY: all clean