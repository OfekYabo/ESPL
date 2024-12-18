#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define VirusStartSize 18
#define VirusSigSize 2
#define VirusNameSize (VirusStartSize - VirusSigSize)
FILE *infile;
FILE *outfile;
bool isLittleEndian;

typedef struct virus
{
    unsigned short SigSize;
    char virusName[VirusNameSize];
    unsigned char *sig;
} virus;

typedef struct link
{
    struct link *nextVirus;
    virus *vir;
} link;

void freeVirus(virus *v)
{
    free(v->sig);
    free(v);
}

void closeFiles()
{
    fclose(infile);
    if (outfile != stdout)
    {
        fclose(outfile);
    }
}

virus *readVirus(FILE *file)
{
    virus *v = (virus *)malloc(sizeof(virus));
    if (fread(v, 1, VirusStartSize, file) != VirusStartSize)
    {
        free(v);
        return NULL;
    }

    // Adjust SigSize based on endianness
    if (!isLittleEndian)
        v->SigSize = (v->SigSize >> 8) | (v->SigSize << 8);

    v->sig = (unsigned char *)malloc(v->SigSize);
    if (fread(v->sig, 1, v->SigSize, file) != v->SigSize)
    {
        freeVirus(v);
        return NULL;
    }
    return v;
}

void printVirus(virus *virus, FILE *output)
{
    fprintf(output, "Virus name: %s\n", virus->virusName);
    fprintf(output, "Virus size: %d\n", virus->SigSize);
    fprintf(output, "signature:\n");
    for (int i = 0; i < virus->SigSize; i++)
    {
        fprintf(output, "%02X ", virus->sig[i]);
        if ((i + 1) % 20 == 0)
        {
            fprintf(output, "\n");
        }
    }
    fprintf(output, "\n");
    fprintf(output, "\n");
}

int main(int argc, char **argv)
{
    if (argc != 2 && argc != 3)
    {
        fprintf(stderr, "Usage: %s <signature file> [output file]\n", argv[0]);
        return 1;
    }

    infile = fopen(argv[1], "rb");
    if (infile == NULL)
    {
        fprintf(stderr, "Error opening input file: %s\n", argv[1]);
        return 1;
    }

    outfile = (argc == 3) ? fopen(argv[2], "w") : stdout;
    if (argc == 3 && outfile == NULL)
    {
        fprintf(stderr, "Error opening output file: %s\n", argv[2]);
        fclose(infile);
        return 1;
    }

    char magicNumber[4];
    if (fread(magicNumber, 1, 4, infile) != 4 || (strncmp(magicNumber, "VIRL", 4) != 0 && strncmp(magicNumber, "VIRB", 4) != 0))
    {
        fprintf(stderr, "Error: Invalid magic number\n");
        fclose(infile);
        if (outfile != stdout)
        {
            fclose(outfile);
        }
        return 1;
    }
    isLittleEndian = (strncmp(magicNumber, "VIRL", 4) == 0);
    virus *v;
    while ((v = readVirus(infile)) != NULL)
    {
        printVirus(v, outfile);
        freeVirus(v);
    }

    closeFiles();
    return 0;
}