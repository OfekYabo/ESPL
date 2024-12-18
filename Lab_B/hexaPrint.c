#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BUFFER_SIZE 13

void PrintHex(unsigned char *buffer, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Input Error, program should get 1 argument- filename");
        exit(1);
    }

    FILE *infile = fopen(argv[1], "rb");
    if (infile == NULL)
    {
        fprintf(stderr, "Error opening input file: %s\n", argv[1]);
        exit(1);
    }

    unsigned char buffer[BUFFER_SIZE];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, infile)) > 0)
    {
        PrintHex(buffer, bytesRead);
    }

    fclose(infile);
    return 0;
}