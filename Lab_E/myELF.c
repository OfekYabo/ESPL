#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <elf.h>
#include <stdbool.h>

// helped by copilot

#define MAX_FILES 2

typedef struct
{
    int fd;
    char filename[256];
    void *map_start;
    size_t file_size;
    Elf32_Ehdr *header;
    Elf32_Shdr *section_headers;
    const char *section_str_table;
    //Elf32_Shdr *strtab;
    const char *sym_str_table;
} ELFFile;

typedef struct
{
    char *name;
    void (*func)();
} MenuOption;

int debug_mode = 0;
ELFFile elf_files[MAX_FILES] = {
    {-1, "", NULL, 0, NULL, NULL, NULL, NULL},
    {-1, "", NULL, 0, NULL, NULL, NULL, NULL}};

void toggle_debug_mode()
{
    debug_mode = !debug_mode;
    printf("Debug mode %s\n", debug_mode ? "on" : "off");
}

// Initialize ELF file struct with extra information
void extra_initialize_elf_file(ELFFile *elf_file, const char *filename)
{
    strncpy(elf_file->filename, filename, 256); // Copy filename to struct

    elf_file->section_headers = (Elf32_Shdr *)(elf_file->map_start + elf_file->header->e_shoff); // Get section headers
    elf_file->section_str_table = (char *)(elf_file->map_start + elf_file->section_headers[elf_file->header->e_shstrndx].sh_offset); // Get section string table

    // Find the string table
    //elf_file->strtab = NULL;
    elf_file->sym_str_table = NULL;
    for (int j = 0; j < elf_file->header->e_shnum; j++)
    {
        if (elf_file->section_headers[j].sh_type == SHT_STRTAB && strcmp(elf_file->section_str_table + elf_file->section_headers[j].sh_name, ".strtab") == 0)
        {
            Elf32_Shdr *strtab = &elf_file->section_headers[j]; // Get string table
            elf_file->sym_str_table = (char *)(elf_file->map_start + strtab->sh_offset); // Get symbol string table
            //elf_file->strtab = &elf_file->section_headers[j]; // Get string table
            //elf_file->sym_str_table = (char *)(elf_file->map_start + elf_file->strtab->sh_offset); // Get symbol string table
            break;
        }
    }
}

void examine_elf_file()
{
    char filename[256];
    printf("\n");
    printf("Enter ELF file name: ");
    scanf("%s", filename);

    for (int i = 0; i < MAX_FILES; i++)
    {
        if (elf_files[i].fd == -1)
        {
            elf_files[i].fd = open(filename, O_RDONLY);
            if (elf_files[i].fd < 0)
            {
                perror("Error opening file");
                return;
            }

            elf_files[i].file_size = lseek(elf_files[i].fd, 0, SEEK_END);
            lseek(elf_files[i].fd, 0, SEEK_SET);

            elf_files[i].map_start = mmap(NULL, elf_files[i].file_size, PROT_READ, MAP_PRIVATE, elf_files[i].fd, 0);
            if (elf_files[i].map_start == MAP_FAILED)
            {
                perror("Error mapping file");
                close(elf_files[i].fd);
                elf_files[i].fd = -1;
                return;
            }

            elf_files[i].header = (Elf32_Ehdr *)elf_files[i].map_start;
            printf("Magic: %c%c%c\n", elf_files[i].header->e_ident[1], elf_files[i].header->e_ident[2], elf_files[i].header->e_ident[3]);

            if (strncmp((char *)elf_files[i].header->e_ident, ELFMAG, SELFMAG) != 0)
            {
                printf("Not an ELF file\n");
                munmap(elf_files[i].map_start, elf_files[i].file_size);
                close(elf_files[i].fd);
                elf_files[i].fd = -1;
                return;
            }

            extra_initialize_elf_file(&elf_files[i], filename);

            printf("Entry point: 0x%x\n", elf_files[i].header->e_entry);
            printf("Data encoding: %d\n", elf_files[i].header->e_ident[EI_DATA]);
            printf("Section header offset: %d\n", elf_files[i].header->e_shoff);
            printf("Number of section headers: %d\n", elf_files[i].header->e_shnum);
            printf("Size of section headers: %d\n", elf_files[i].header->e_shentsize);
            printf("Program header offset: %d\n", elf_files[i].header->e_phoff);
            printf("Number of program headers: %d\n", elf_files[i].header->e_phnum);
            printf("Size of program headers: %d\n", elf_files[i].header->e_phentsize);

            if (debug_mode)
            {
                printf("Debug: File size: %zu bytes\n", elf_files[i].file_size);
                printf("Debug: File descriptor: %d\n", elf_files[i].fd);
                printf("\n");
            }

            return;
        }
    }

    printf("Cannot open more than %d ELF files\n", MAX_FILES);
}

void print_section_names()
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (elf_files[i].fd == -1)
        {
            printf("No ELF file is currently opened in slot %d\n", i);
            continue;
        }

        printf("\n");
        printf("File: %s\n", elf_files[i].filename);

        if (debug_mode)
        {
            printf("Debug: shstrndx: %d\n", elf_files[i].header->e_shstrndx);
        }

        printf("Section Headers:\n");
        printf("  [Nr] Name              Type            Addr     Off\n");

        for (int j = 0; j < elf_files[i].header->e_shnum; j++)
        {
            printf("  [%2d] %-17s %-15x %08x %06x\n",
                   j,
                   elf_files[i].section_str_table + elf_files[i].section_headers[j].sh_name,
                   elf_files[i].section_headers[j].sh_type,
                   elf_files[i].section_headers[j].sh_addr,
                   elf_files[i].section_headers[j].sh_offset);

            if (debug_mode)
            {
                printf("Debug: Section %d name offset: 0x%08x\n", j, elf_files[i].section_headers[j].sh_name);
            }
        }
    }
}

void print_symbols()
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (elf_files[i].fd == -1)
        {
            printf("No ELF file is currently opened in slot %d\n", i);
            continue;
        }

        bool found_symtab = false;

        printf("\n");
        printf("File: %s\n", elf_files[i].filename);

        if (elf_files[i].sym_str_table == NULL)
        {
            printf("No symbol string table found in file: %s\n", elf_files[i].filename);
            continue;
        }

        for (int j = 0; j < elf_files[i].header->e_shnum; j++)
        {
            if (elf_files[i].section_headers[j].sh_type == SHT_SYMTAB)
            {
                found_symtab = true;
                Elf32_Shdr *symtab = &elf_files[i].section_headers[j];
                Elf32_Sym *symbols = (Elf32_Sym *)(elf_files[i].map_start + symtab->sh_offset);
                int num_symbols = symtab->sh_size / symtab->sh_entsize;

                if (debug_mode)
                {
                    printf("Debug: Symbol Table size: %d bytes\n", symtab->sh_size);
                    printf("Debug: Number of symbols: %d\n", num_symbols);
                }

                printf("Symbol Table:\n");
                printf("  [Nr] Value    Section Index Section Name        Symbol Name\n");

                for (int k = 0; k < num_symbols; k++)
                {
                    const char *symbol_name;
                    if (symbols[k].st_name >= symtab->sh_size)
                    {
                        printf("Error: Symbol name offset out of bounds\n");
                        continue;
                    }
                    else if (symbols[k].st_name == 0)
                    {
                        // Symbol name offset is 0, use section name
                        symbol_name = elf_files[i].section_str_table + elf_files[i].section_headers[symbols[k].st_shndx].sh_name;
                    }
                    else
                    {
                        // Use symbol string table
                        symbol_name = elf_files[i].sym_str_table + symbols[k].st_name;
                    }

                    const char *section_name;

                    if (symbols[k].st_shndx == SHN_UNDEF)
                    {
                        section_name = "UNDEF";
                    }
                    else if (symbols[k].st_shndx == SHN_ABS)
                    {
                        section_name = "ABS";
                    }
                    else if (symbols[k].st_shndx == SHN_COMMON)
                    {
                        section_name = "COMMON";
                    }
                    else if (symbols[k].st_shndx >= elf_files[i].header->e_shnum)
                    {
                        printf("Error: Section index out of bounds- st_shndx: %d\n", symbols[k].st_shndx);
                        continue;
                    }
                    else
                    {
                        section_name = elf_files[i].section_str_table + elf_files[i].section_headers[symbols[k].st_shndx].sh_name;
                    }

                    printf("  [%2d] %08x %14d %-17s %s\n",
                           k,
                           symbols[k].st_value,
                           symbols[k].st_shndx,
                           section_name,
                           symbol_name);

                    if (debug_mode)
                    {
                        printf("Debug: Symbol %d name offset: 0x%08x\n", k, symbols[k].st_name);
                        printf("Debug: Symbol %d name: %s\n\n", k, symbol_name);
                    }
                }
            }
        }

        if (!found_symtab)
        {
            printf("No symbol table found in file: %s\n", elf_files[i].filename);
        }
    }
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
        if (elf_files[i].map_start != NULL)
        {
            munmap(elf_files[i].map_start, elf_files[i].file_size);
        }
        if (elf_files[i].fd != -1)
        {
            close(elf_files[i].fd);
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
    printf("\n");
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