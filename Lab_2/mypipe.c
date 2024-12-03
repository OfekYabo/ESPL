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
        exit(1);
    }
    read_fd = pipefd[0];
    write_fd = pipefd[1];
    // Fork a new process
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork() error");
        return 1;
    } else if (pid == 0) {
        // Child process
        childExecute();
        exit(0);
    } else {
        // Parent process
        parentExecute();
        wait(NULL); // Wait for child process to finish
        exit(0);
    }       
}


void childExecute() {
    // Close unused read of the pipe
    close(read_fd);

    // Write a message to the pipe
    const char *message = "hello";
    write(write_fd, message, strlen(message));
    close(write_fd);
}

void parentExecute() {
    // Close unused write of the pipe
    close(write_fd);

    // Read the message from the pipe
    char buffer[6];
    read(read_fd, buffer, 5);
    buffer[5] = '\0'; // add Null-terminate to the string
    printf("Received from child: %s\n", buffer);
    close(read_fd);
}

