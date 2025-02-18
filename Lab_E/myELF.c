#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <elf.h>

// helped by copilot

#define MAX_FILES 2

typedef struct
{
    char *name;
    void (*func)();
} MenuOption;

int debug_mode = 0;
int fd[MAX_FILES] = {-1, -1};
void *map_start[MAX_FILES] = {NULL, NULL};
size_t file_size[MAX_FILES] = {0, 0};

void toggle_debug_mode()
{
    debug_mode = !debug_mode;
    printf("Debug mode %s\n", debug_mode ? "on" : "off");
}

void examine_elf_file()
{
    char filename[256];
    printf("Enter ELF file name: ");
    scanf("%s", filename);

    for (int i = 0; i < MAX_FILES; i++)
    {
        if (fd[i] == -1)
        {
            fd[i] = open(filename, O_RDONLY);
            if (fd[i] < 0)
            {
                perror("Error opening file");
                return;
            }

            file_size[i] = lseek(fd[i], 0, SEEK_END);
            lseek(fd[i], 0, SEEK_SET);

            /**
             *       - NULL: The kernel chooses the address at which to create the mapping.
             *       - PROT_READ: Pages may be read.
             *       - MAP_PRIVATE: Create a private copy-on-write mapping.
             *       - 0: The offset in the file where the mapping starts.
             */
            map_start[i] = mmap(NULL, file_size[i], PROT_READ, MAP_PRIVATE, fd[i], 0);
            if (map_start[i] == MAP_FAILED)
            {
                perror("Error mapping file");
                close(fd[i]);
                fd[i] = -1;
                return;
            }

            Elf32_Ehdr *header = (Elf32_Ehdr *)map_start[i];
            printf("Magic: %c%c%c\n", header->e_ident[1], header->e_ident[2], header->e_ident[3]);
            
            if (strncmp((char *)header->e_ident, ELFMAG, SELFMAG) != 0)
            {
                printf("Not an ELF file\n");
                munmap(map_start[i], file_size[i]);
                close(fd[i]);
                fd[i] = -1;
                return;
            }

            printf("Entry point: 0x%x\n", header->e_entry);
            printf("Data encoding: %d\n", header->e_ident[EI_DATA]);
            printf("Section header offset: %d\n", header->e_shoff);
            printf("Number of section headers: %d\n", header->e_shnum);
            printf("Size of section headers: %d\n", header->e_shentsize);
            printf("Program header offset: %d\n", header->e_phoff);
            printf("Number of program headers: %d\n", header->e_phnum);
            printf("Size of program headers: %d\n", header->e_phentsize);

            return;
        }
    }

    printf("Cannot open more than %d ELF files\n", MAX_FILES);
}

void print_section_names()
{
    printf("Not implemented yet\n");
}

void print_symbols()
{
    printf("Not implemented yet\n");
}

void check_files_for_merge()
{
    printf("Not implemented yet\n");
}

void merge_elf_files()
{
    printf("Not implemented yet\n");
}

void quit()
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (map_start[i] != NULL)
        {
            munmap(map_start[i], file_size[i]);
        }
        if (fd[i] != -1)
        {
            close(fd[i]);
        }
    }
    printf("Exiting...\n");
    exit(0);
}

MenuOption menu[] = {
    {"Toggle Debug Mode", toggle_debug_mode},
    {"Examine ELF File", examine_elf_file},
    {"Print Section Names", print_section_names},
    {"Print Symbols", print_symbols},
    {"Check Files for Merge", check_files_for_merge},
    {"Merge ELF Files", merge_elf_files},
    {"Quit", quit},
    {NULL, NULL}};

void display_menu()
{
    printf("Choose action:\n");
    for (int i = 0; menu[i].name != NULL; i++)
    {
        printf("%d-%s\n", i, menu[i].name);
    }
}

int main()
{
    while (1)
    {
        display_menu();
        int choice;
        scanf("%d", &choice);
        if (choice >= 0 && choice < sizeof(menu) / sizeof(MenuOption) - 1)
        {
            menu[choice].func();
        }
        else
        {
            printf("Invalid choice\n");
        }
    }
    return 0;
}