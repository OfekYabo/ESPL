#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "LineParser.h"

#define MAX_INPUT_SIZE 2048

void execute(cmdLine *pCmdLine);
void sendSignal(int signal);

int main(int argc, char **argv) {
    char cwd[PATH_MAX];
    char input[MAX_INPUT_SIZE];
    cmdLine *parsedCmdLine;
    int debug = 0;

    // Check for -d flag
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            debug = 1;
        }
    }

    while (1) {
        // Get the current working directory
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s$ ", cwd);
        } else {
            perror("getcwd() error"); //print error to stderr and the last error
            return 1;
        }
        // Read user input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("fgets() error");
            return 1;
        }
        // Parse the input
        parsedCmdLine = parseCmdLines(input);
        if (parsedCmdLine == NULL) {
            continue; // If parsing fails, continue to the next iteration
        }
        // Check for "quit" command
        if (strcmp(parsedCmdLine->arguments[0], "quit") == 0) {
            freeCmdLines(parsedCmdLine);
            break; // Exit the loop and terminate the shell
        }
        // Check for "cd" command
        if (strcmp(parsedCmdLine->arguments[0], "cd") == 0) {
            if (parsedCmdLine->argCount < 2) {
                fprintf(stderr, "cd: missing argument\n");
            } else {
                if (chdir(parsedCmdLine->arguments[1]) != 0) {
                    perror("cd error");
                }
            }
            freeCmdLines(parsedCmdLine);
            continue; // Skip forking and continue to the next iteration
        }






        // Check for "stop" command
        if ((strcmp(parsedCmdLine->arguments[0], "stop") == 0)) {
            if (parsedCmdLine->argCount < 2) {
                fprintf(stderr, "stop: missing argument\n");
            } else {
                pid_t pid = atoi(parsedCmdLine->arguments[1]);
                if (kill(pid, SIGSTOP) == -1) {
                    perror("stop error");
                }
            }
            freeCmdLines(parsedCmdLine);
            continue; // Skip forking and continue to the next iteration
        }

        // Check for "wake" command
        if (strcmp(parsedCmdLine->arguments[0], "wake") == 0) {
            if (parsedCmdLine->argCount < 2) {
                fprintf(stderr, "wake: missing argument\n");
            } else {
                pid_t pid = atoi(parsedCmdLine->arguments[1]);
                if (kill(pid, SIGCONT) == -1) {
                    perror("wake error");
                }
            }
            freeCmdLines(parsedCmdLine);
            continue; // Skip forking and continue to the next iteration
        }

        // Check for "term" command
        if (strcmp(parsedCmdLine->arguments[0], "term") == 0) {
            if (parsedCmdLine->argCount < 2) {
                fprintf(stderr, "term: missing argument\n");
            } else {
                pid_t pid = atoi(parsedCmdLine->arguments[1]);
                if (kill(pid, SIGINT) == -1) {
                    perror("term error");
                }
            }
            freeCmdLines(parsedCmdLine);
            continue; // Skip forking and continue to the next iteration
        }







        // Fork a new process
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork() error");
            freeCmdLines(parsedCmdLine);
            continue;
        } else if (pid == 0) {
            // Child process
            execute(parsedCmdLine);
        } else {
            // Parent process
            if (debug) {
                fprintf(stderr, "PID: %d\n", pid);
                fprintf(stderr, "Executing command: %s\n", parsedCmdLine->arguments[0]);
            }
            if (parsedCmdLine->blocking) {
                waitpid(pid, NULL, 0); // Wait for the child process to complete if blocking
            }
        }       

        freeCmdLines(parsedCmdLine);
    }

    return 0;
}

void execute(cmdLine *pCmdLine) {
    if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
        perror("execvp() error");
        _exit(1); // Exit the child process abnormally
    }
}