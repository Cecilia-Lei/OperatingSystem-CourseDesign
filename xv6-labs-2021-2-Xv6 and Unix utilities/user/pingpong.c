#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
    // Parent to child communication pipe
    int fd[2];

    if (pipe(fd) == -1) {
        fprintf(2, "Error: Failed to create pipe\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "Error: Fork failed\n");
        exit(1);
    } 
    else if (pid == 0) {
        // Child process
        close(fd[1]); // Close unused write end
        
        char buffer[1];
        if (read(fd[0], buffer, 1) != 1) {
            fprintf(2, "Error: Read from pipe failed\n");
            exit(1);
        }
        fprintf(1, "%d: received ping\n", getpid());
        
        close(fd[0]); // Close read end
        exit(0);
    } 
    else {
        // Parent process
        close(fd[0]); // Close unused read end
        
        char buffer[1] = {'a'};
        if (write(fd[1], buffer, 1) != 1) {
            fprintf(2, "Error: Write to pipe failed\n");
            exit(1);
        }
        close(fd[1]); // Close write end
        
        // Wait for child process to complete
        wait(0);
        
        fprintf(1, "%d: received pong\n", getpid());
        exit(0);
    }
}
