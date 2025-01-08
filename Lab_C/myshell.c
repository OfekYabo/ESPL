#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "LineParser.h"
#include <stdbool.h>
#define MAX_INPUT_SIZE 2048

// my code, helped by copilot
int debug;
int read_fd;
int write_fd;
void execute(cmdLine *pCmdLine);
void executeCommand(cmdLine *pCmdLine);
void executePipe(cmdLine *leftCmd, cmdLine *rightCmd);
void handleRedirection(cmdLine *pCmdLine);
void debugPrint(pid_t pid, cmdLine *cmdLine);
bool checkRedirection(cmdLine *pCmdLine);

int main(int argc, char **argv)
{
    char cwd[PATH_MAX];
    char input[MAX_INPUT_SIZE];
    cmdLine *parsedCmdLine;
    debug = 0;

    // Check for -d flag
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-d") == 0)
        {
            debug = 1;
        }
    }

    while (1)
    {
        // Get the current working directory
        if (isatty(STDIN_FILENO))
        {
            if (getcwd(cwd, sizeof(cwd)) != NULL)
            {
                printf("%s$ ", cwd);
            }
            else
            {
                perror("getcwd() error"); // print to stderr the last error
                return 1;
            }
        }
        // Read user input
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            perror("fgets() error");
            return 1;
        }
        // Parse the input
        parsedCmdLine = parseCmdLines(input);
        if (parsedCmdLine == NULL)
        {
            continue; // If parsing fails, continue to the next iteration
        }
        // Check "quit" command
        if (strcmp(parsedCmdLine->arguments[0], "quit") == 0)
        {
            freeCmdLines(parsedCmdLine);
            break; // Exit the loop and terminate the shell
        }
        // Check "cd" command
        if (strcmp(parsedCmdLine->arguments[0], "cd") == 0)
        {
            if (parsedCmdLine->argCount < 2)
            {
                fprintf(stderr, "cd: missing argument\n");
            }
            else
            {
                if (chdir(parsedCmdLine->arguments[1]) != 0)
                {
                    perror("cd error");
                }
            }
            freeCmdLines(parsedCmdLine);
            continue; // Skip forking and continue to the next iteration
        }

        // Check for "stop" command
        if ((strcmp(parsedCmdLine->arguments[0], "stop") == 0))
        {
            if (parsedCmdLine->argCount < 2)
            {
                fprintf(stderr, "stop: missing argument\n");
            }
            else
            {
                pid_t pid = atoi(parsedCmdLine->arguments[1]);
                if (kill(pid, SIGKILL) == -1)
                {
                    perror("stop error");
                }
            }
            freeCmdLines(parsedCmdLine);
            continue; // Skip forking and continue to the next iteration
        }

        // Check for "wake" command
        if (strcmp(parsedCmdLine->arguments[0], "wake") == 0)
        {
            if (parsedCmdLine->argCount < 2)
            {
                fprintf(stderr, "wake: missing argument\n");
            }
            else
            {
                pid_t pid = atoi(parsedCmdLine->arguments[1]);
                if (kill(pid, SIGCONT) == -1)
                {
                    perror("wake error");
                }
            }
            freeCmdLines(parsedCmdLine);
            continue; // Skip forking and continue to the next iteration
        }

        // Check for "term" command
        if (strcmp(parsedCmdLine->arguments[0], "term") == 0)
        {
            if (parsedCmdLine->argCount < 2)
            {
                fprintf(stderr, "term: missing argument\n");
            }
            else
            {
                pid_t pid = atoi(parsedCmdLine->arguments[1]);
                if (kill(pid, SIGINT) == -1)
                {
                    perror("term error");
                }
            }
            freeCmdLines(parsedCmdLine);
            continue; // Skip forking and continue to the next iteration
        }

        if (parsedCmdLine->next)
        {
            if (checkRedirection(parsedCmdLine))
            {
                executePipe(parsedCmdLine, parsedCmdLine->next);
            }
        }
        else
        {
            executeCommand(parsedCmdLine);
        }

        freeCmdLines(parsedCmdLine);
    }
    return 0;
}





void debugPrint(pid_t pid, cmdLine *cmdLine)
{
    if (debug)
    {
        fprintf(stderr, "PID: %d\n", pid);
        fprintf(stderr, "Executing command: %s\n", cmdLine->arguments[0]);
    }
}

bool checkRedirection(cmdLine *pCmdLine)
{
    bool isValid = true;
    if (pCmdLine->outputRedirect)
    {
        fprintf(stderr, "Error: Output redirection on the left-hand-side of the pipe is not allowed.\n");
        isValid = false;
    }
    else if (pCmdLine->next->inputRedirect)
    {
        fprintf(stderr, "Error: Input redirection on the right-hand-side of the pipe is not allowed.\n");
        isValid = false;
    }
    return isValid;
}

void handleRedirection(cmdLine *pCmdLine)
{
    int inputFd;
    int outputFd;
    // Handle input redirection
    if (pCmdLine->inputRedirect)
    {
        close(STDIN_FILENO);
        inputFd = open(pCmdLine->inputRedirect, O_RDONLY);
        if (inputFd == -1)
        {
            perror("open inputRedirect error");
            _exit(1);
        }
    }

    // Handle output redirection
    if (pCmdLine->outputRedirect)
    {
        close(STDOUT_FILENO);
        outputFd = open(pCmdLine->outputRedirect, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (outputFd == -1)
        {
            perror("open outputRedirect error");
            _exit(1);
        }
    }
}

void execute(cmdLine *pCmdLine)
{
    handleRedirection(pCmdLine);
    if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1)
    {
        perror("execvp() error");
        _exit(1); // Exit the child process abnormally
    }
}

void executeCommand(cmdLine *pCmdLine)
{
    pid_t pid = fork();
    if (pid == 0)
    { // Child process
        execute(pCmdLine);
    }
    else if (pid > 0)
    { // Parent process
        debugPrint(pid, pCmdLine);
        if (pCmdLine->blocking)
        {
            waitpid(pid, NULL, 0); // Wait for the child process to complete if blocking
        }
    }
    else
    {
        perror("fork() error");
    }
}

void executePipe(cmdLine *leftCmd, cmdLine *rightCmd)
{
    int pipefd[2];
    pid_t cpid_L, cpid_R;

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    read_fd = pipefd[0];
    write_fd = pipefd[1];

    cpid_L = fork();

    if (cpid_L == 0)
    { // First child process
        close(STDOUT_FILENO);
        dup(write_fd);
        close(write_fd);
        close(read_fd);
        execute(leftCmd);
    }
    else if (cpid_L > 0)
    { // Parent process
        debugPrint(cpid_L, leftCmd);
        close(write_fd);
        cpid_R = fork();

        if (cpid_R == 0)
        { // Second child process
            close(STDIN_FILENO);
            dup(read_fd);
            close(read_fd);
            execute(rightCmd);
        }
        else if (cpid_R > 0)
        { // Parent process
            debugPrint(cpid_R, rightCmd);
            close(read_fd);
            waitpid(cpid_L, NULL, 0);
            waitpid(cpid_R, NULL, 0);
        }
        else
        {
            perror("fork rightCmd");
            exit(EXIT_FAILURE);
        }
    }
    else
    { 
        perror("fork leftCmd");
        exit(EXIT_FAILURE);
    }
}