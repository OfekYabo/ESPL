#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int pipefd[2];
int read_fd;
int write_fd;
void childExecute();
void parentExecute();


int main() {
    

    // Create the pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    read_fd = pipefd[0];
    write_fd = pipefd[1];
    // Fork a new process
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork() error");
        exit(1);
    } else if (pid == 0) {
        // Child process
        childExecute();
        exit(EXIT_SUCCESS); // Ensure child process terminates
    } else {
        // Parent process
        parentExecute();
        wait(NULL); // Wait for child process to finish
        exit(EXIT_SUCCESS); // Ensure parent process terminates
    }       
}

void childExecute() {
    // Close the unused read end of the pipe
    close(read_fd);

    // Write a message to the pipe
    const char *message = "hello";
    write(write_fd, message, strlen(message));

    // Close the write end of the pipe
    close(write_fd);
}

void parentExecute() {
    // Close the unused write end of the pipe
    close(write_fd);

    // Read the message from the pipe
    char buffer[6];
    read(read_fd, buffer, 5);
    buffer[5] = '\0'; // Null-terminate the string

    // Print the message
    printf("Received from child: %s\n", buffer);

    // Close the read end of the pipe
    close(read_fd);
}

