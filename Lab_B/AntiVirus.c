#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define VirusStartSize 18
#define VirusSigSize 2
#define VirusNameSize (VirusStartSize - VirusSigSize)
#define Input_Len 10

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

typedef void (*MenuFunction)(void);

typedef struct
{
    char *name;
    MenuFunction func;
} MenuEntry;

FILE *globalInfile;
FILE *globalOutfile;
bool isLittleEndian;
char *fixFileName = NULL;
bool neutralize = false;
link *virusList = NULL;

void handle_arguments(int argc, char **argv)
{
    globalInfile = stdin;
    globalOutfile = stdout;

    for (int i = 1; i < argc; i++)
    {
        if (strncmp(argv[i], "-i", 2) == 0)
        {
            globalInfile = fopen(argv[i] + 2, "r");
            if (globalInfile == NULL)
            {
                fprintf(stderr, "Error opening input file: %s\n", argv[i] + 2);
                exit(1);
            }
        }
        else if (strncmp(argv[i], "-o", 2) == 0)
        {
            globalOutfile = fopen(argv[i] + 2, "w");
            if (globalOutfile == NULL)
            {
                fprintf(stderr, "Error opening output file: %s\n", argv[i] + 2);
                exit(1);
            }
        }
    }
}

void freeVirus(virus *v)
{
    free(v->sig);
    free(v);
}

void close_global_files()
{
    if (globalInfile != stdin)
    {
        fclose(globalInfile);
    }
    if (globalOutfile != stdout)
    {
        fclose(globalOutfile);
    }
    else
    {
        fflush(stdout);
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

void list_print(link *virus_list, FILE *output)
{
    link *current = virus_list;
    while (current != NULL)
    {
        printVirus(current->vir, output);
        current = current->nextVirus;
    }
}

link *list_append(link *virus_list, virus *data)
{
    link *newLink = (link *)malloc(sizeof(link));
    newLink->vir = data;
    newLink->nextVirus = virus_list;
    return newLink;
}

void list_free(link *virus_list)
{
    link *current = virus_list;
    while (current != NULL)
    {
        link *next = current->nextVirus;
        freeVirus(current->vir);
        free(current);
        current = next;
    }
}

void neutralize_virus(char *fileName, int signatureOffset) {
    FILE *file = fopen(fileName, "r+b");
    if (file == NULL) {
        perror("Error opening file for writing");
        return;
    }

    // Move to the position of the virus signature
    fseek(file, signatureOffset, SEEK_SET);

    // Write the RET instruction (0xC3)
    unsigned char retInstruction = 0xC3;
    fwrite(&retInstruction, 1, 1, file);
    fprintf(globalOutfile, "Virus neutralized\n");
    fclose(file);
}

void detect_virus(char *buffer, unsigned int size, link *virus_list) {
    link *current = virus_list;
    while (current != NULL) {
        virus *v = current->vir;
        for (unsigned int i = 0; i <= size - v->SigSize; i++) {
            if (memcmp(buffer + i, v->sig, v->SigSize) == 0) {
                fprintf(globalOutfile, "Virus detected:\n");
                fprintf(globalOutfile, "Starting byte location: %u\n", i);
                fprintf(globalOutfile, "Virus name: %s\n", v->virusName);
                fprintf(globalOutfile, "Virus size: %u\n", v->SigSize);

                // when neutralize flag is on
                if (fixFileName != NULL) {
                    neutralize_virus(fixFileName, i);
                }
            }
        }
        current = current->nextVirus;
    }
}

void load_signatures(void)
{
    char filename[256];
    fprintf(globalOutfile, "Enter signature file name: ");
    if (fgets(filename, sizeof(filename), globalInfile) == NULL) {
        fprintf(globalOutfile, "Error reading filename\n");
        return;
    }
    filename[strcspn(filename, "\n")] = '\0'; // Remove newline character

    FILE *infile = fopen(filename, "rb");
    if (infile == NULL)
    {
        perror("Error opening file");
        return;
    }

    char magicNumber[4];
    if (fread(magicNumber, 1, 4, infile) != 4 || (strncmp(magicNumber, "VIRL", 4) != 0 && strncmp(magicNumber, "VIRB", 4) != 0))
    {
        fprintf(stderr, "Error: Invalid magic number\n");
        fclose(infile);
        return;
    }
    isLittleEndian = (strncmp(magicNumber, "VIRL", 4) == 0);

    virus *v;
    while ((v = readVirus(infile)) != NULL)
    {
        virusList = list_append(virusList, v);
    }

    fclose(infile);
}

void print_signatures(void)
{
    list_print(virusList, globalOutfile);
}

void detect_viruses(void)
{
    char *filename = (char *)malloc(256 * sizeof(char));
    if (filename == NULL) {
        fprintf(globalOutfile, "Error allocating memory for filename\n");
        return;
    }

    fprintf(globalOutfile, "Enter suspected file name: ");
    
    if (fgets(filename, 256, globalInfile) == NULL) {
        fprintf(globalOutfile, "Error reading filename\n");
        free(filename);
        return;
    }
    filename[strcspn(filename, "\n")] = '\0'; // Remove newline character

    FILE *suspectedFile = fopen(filename, "rb");
    if (suspectedFile == NULL)
    {
        perror("Error opening file");
        free(filename);
        return;
    }

    if (neutralize) 
        fixFileName = filename;

    const unsigned int bufferSize = 10000;
    char buffer[bufferSize];

    fseek(suspectedFile, 0, SEEK_END);
    long fileSize = ftell(suspectedFile);
    fseek(suspectedFile, 0, SEEK_SET);

    unsigned int sizeToRead = (fileSize < bufferSize) ? fileSize : bufferSize;
    fread(buffer, 1, sizeToRead, suspectedFile);
    fclose(suspectedFile);

    detect_virus(buffer, sizeToRead, virusList);
    free(filename); // Free allocated memory
}

void fix_file(void)
{
    neutralize = true;
    detect_viruses();
    neutralize = false;
    fixFileName = NULL;
}

void quit(void) {
    fprintf(globalOutfile, "Quit\n");
    list_free(virusList);
    close_global_files();
    exit(0);
}

int main(int argc, char **argv)
{
    handle_arguments(argc, argv);

    MenuEntry menu[] = {
        {"Load signatures", load_signatures},
        {"Print signatures", print_signatures},
        {"Detect viruses", detect_viruses},
        {"Fix file", fix_file},
        {"Quit", quit},
        {NULL, NULL}};

    int bound = 0;
    while (menu[bound].name != NULL)
    {
        bound++;
    }

    // my code, advised  by copilot to improve it

    char option_str[Input_Len];
    while (1)
    {
        fprintf(globalOutfile, "Select operation from the following menu by number:\n");
        for (int i = 0; i < bound; i++)
        {
            fprintf(globalOutfile, "%i) %s\n", i + 1, menu[i].name); // 1-5 instead of 0-4
        }
        fprintf(globalOutfile, "Option : ");

        if (fgets(option_str, sizeof(option_str), globalInfile) == NULL)
        {
            break;
        }

        char *endptr;
        int option = strtol(option_str, &endptr, 10);
        if (endptr == option_str || *endptr != '\n')
        {
            fprintf(globalOutfile, "Invalid option\n");
            continue;
        }

        option--; // 1-5 instead of 0-4

        if (option >= 0 && option < bound)
        {
            menu[option].func();
        }
        else
        {
            fprintf(globalOutfile, "Not within bounds\n");
        }
    }
    quit();
}