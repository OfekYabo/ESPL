#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int read_fd;
int write_fd;

int main()
{
    int pipefd[2];
    pid_t cpid1, cpid2;

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    read_fd = pipefd[0];
    write_fd = pipefd[1];

    fprintf(stderr, "(parent_process>forking...)\n");
    cpid1 = fork();
    if (cpid1 == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid1 == 0)
    { // First child process
        fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe...)\n");
        close(STDOUT_FILENO);
        dup(write_fd);
        close(write_fd);
        close(read_fd);
        fprintf(stderr, "(child1>going to execute cmd: ls -l)\n");
        char *args1[] = {"ls", "-l", NULL};
        execvp(args1[0], args1);
        perror("execvp child 1");
        exit(EXIT_FAILURE);
    }
    else
    { // Parent process
        fprintf(stderr, "(parent_process>created process with id: %d)\n", cpid1);
        close(write_fd); //will not receive an EOF on the read end of the pipe. So tail will keep waiting for more input and wont terminate as expected.
        fprintf(stderr, "(parent_process>closing the write end of the pipe...)\n");

        fprintf(stderr, "(parent_process>forking...)\n");
        cpid2 = fork();
        if (cpid2 == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (cpid2 == 0)
        { // Second child process
            fprintf(stderr, "(child2>redirecting stdin to the read end of the pipe...)\n");
            close(STDIN_FILENO);
            dup(read_fd);
            close(read_fd);
            fprintf(stderr, "(child2>going to execute cmd: tail -n 2)\n");
            char *args2[] = {"tail", "-n", "2", NULL};
            execvp(args2[0], args2);
            perror("execvp child 2");
            exit(EXIT_FAILURE);
        }
        else
        { // Parent process
            fprintf(stderr, "(parent_process>created process with id: %d)\n", cpid2);
            close(read_fd); //good practice to close unused file descriptors to avoid resource leaks.
            fprintf(stderr, "(parent_process>closing the read end of the pipe...)\n");

            fprintf(stderr, "(parent_process>waiting for child processes to terminate...)\n");
            waitpid(cpid1, NULL, 0);
            waitpid(cpid2, NULL, 0);
            fprintf(stderr, "(parent_process>exiting...)\n");
        }
    }

    return 0;
}