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
#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0

// my code, helped by copilot
int debug;
int read_fd;
int write_fd;
process* process_list = NULL;
void execute(cmdLine *pCmdLine);
void executeCommand(cmdLine *pCmdLine);
void executePipe(cmdLine *leftCmd, cmdLine *rightCmd);
void handleRedirection(cmdLine *pCmdLine);
void debugPrint(pid_t pid, cmdLine *cmdLine);
bool checkRedirection(cmdLine *pCmdLine);

void addProcess(process** process_list, cmdLine* cmd, pid_t pid);
void printProcessList(process** process_list);
void freeProcessList(process* process_list);
void updateProcessList(process **process_list);
void updateProcessStatus(process* process_list, int pid, int status);

typedef struct process
{
    cmdLine *cmd;         /* the parsed command line*/
    pid_t pid;            /* the process id that is running the command*/
    int status;           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next; /* next process in chain */
} process;

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

        // Check for process management commands
        if (strcmp(parsedCmdLine->arguments[0], "procs") == 0 ||
            strcmp(parsedCmdLine->arguments[0], "stop") == 0 ||
            strcmp(parsedCmdLine->arguments[0], "term") == 0 ||
            strcmp(parsedCmdLine->arguments[0], "wake") == 0)
        {
            handleProcessCommand(parsedCmdLine);
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
    freeProcessList(process_list);
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
        addProcess(&process_list, pCmdLine, pid);
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
    { // Left child process
        close(STDOUT_FILENO);
        dup(write_fd);
        close(write_fd);
        close(read_fd);
        execute(leftCmd);
    }
    else if (cpid_L > 0)
    { // Parent process
        debugPrint(cpid_L, leftCmd);
        addProcess(&process_list, leftCmd, cpid_L);
        close(write_fd);
        cpid_R = fork();

        if (cpid_R == 0)
        { // Right child process
            close(STDIN_FILENO);
            dup(read_fd);
            close(read_fd);
            execute(rightCmd);
        }
        else if (cpid_R > 0)
        { // Parent process
            debugPrint(cpid_R, rightCmd);
            addProcess(&process_list, rightCmd, cpid_R);
            close(read_fd);
            if (leftCmd->blocking)
            {
                waitpid(cpid_L, NULL, 0); // Wait for the first child process to complete if blocking
            }
            if (rightCmd->blocking)
            {
                waitpid(cpid_R, NULL, 0); // Wait for the second child process to complete if blocking
            }
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

void addProcess(process** process_list, cmdLine* cmd, pid_t pid) {
    process* new_process = (process*)malloc(sizeof(process));
    new_process->cmd = cmd;
    new_process->pid = pid;
    new_process->status = RUNNING;
    new_process->next = *process_list;
    *process_list = new_process;
}

void printProcessList(process** process_list) {
    updateProcessList(process_list);
    process* current = *process_list;
    process* prev = NULL;
    int index = 0;
    printf("Index\tPID\t\tStatus\t\tCommand\n");
    while (current != NULL) {
        printf("%d\t%d\t\t%s\t\t", index, current->pid,
               current->status == RUNNING ? "Running" :
               current->status == SUSPENDED ? "Suspended" : "Terminated");
        for (int i = 0; i < current->cmd->argCount; i++) {
            printf("%s ", current->cmd->arguments[i]);
        }
        printf("\n");

        if (current->status == TERMINATED) {
            if (prev == NULL) {
                *process_list = current->next;
                freeCmdLines(current->cmd);
                free(current);
                current = *process_list;
            } else {
                prev->next = current->next;
                freeCmdLines(current->cmd);
                free(current);
                current = prev->next;
            }
        } else {
            prev = current;
            current = current->next;
        }
        index++;
    }
}

void freeProcessList(process* process_list) {
    process* current = process_list;
    while (current != NULL) {
        process* next = current->next;
        freeCmdLines(current->cmd);
        free(current);
        current = next;
    }
}

void updateProcessList(process **process_list) {
    process* current = *process_list;
    int status;
    pid_t result;
    while (current != NULL) {
        result = waitpid(current->pid, &status, WNOHANG | WUNTRACED);
        if (result == -1) {
            perror("waitpid");
        } else if (result > 0) {
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                current->status = TERMINATED;
            } else if (WIFSTOPPED(status)) {
                current->status = SUSPENDED;
            } else if (WIFCONTINUED(status)) {
                current->status = RUNNING;
            }
        }
        current = current->next;
    }
}

void updateProcessStatus(process* process_list, int pid, int status) {
    process* current = process_list;
    while (current != NULL) {
        if (current->pid == pid) {
            current->status = status;
            break;
        }
        current = current->next;
    }
}


void handleProcessCommand(cmdLine* parsedCmdLine) {
    if (strcmp(parsedCmdLine->arguments[0], "procs") == 0) 
    { // Check "procs" command
        printProcessList(&process_list);
    }
    else if ((strcmp(parsedCmdLine->arguments[0], "stop") == 0))
    { // Check for "stop" command
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
            else 
            {
                updateProcessStatus(process_list, pid, SUSPENDED);
            }
        }
    }
    else if (strcmp(parsedCmdLine->arguments[0], "wake") == 0)
    { // Check for "wake" command
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
            else
            {
                updateProcessStatus(process_list, pid, RUNNING);
            }
        }
    }
    else if (strcmp(parsedCmdLine->arguments[0], "term") == 0)
    { // Check for "term" command
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
            else 
            {
                updateProcessStatus(process_list, pid, TERMINATED);
            }
        }
    }
}


