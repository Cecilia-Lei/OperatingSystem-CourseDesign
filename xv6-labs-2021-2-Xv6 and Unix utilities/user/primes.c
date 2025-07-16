#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define begin 2
#define end 35

void sieve(int fd) 
{
    int p, n;
    int pid;
    int pipefd[2];
    
    // Read the first number
    if (read(fd, &p, sizeof(p)) != sizeof(p)) {
        close(fd);
        exit(0);
    }
    
    printf("prime %d\n", p);
    
    // Create a new pipe
    if (pipe(pipefd) < 0) {
        close(fd);
        exit(1);
    }
    
    pid = fork();
    if (pid < 0) {
        close(fd);
        close(pipefd[0]);
        close(pipefd[1]);
        exit(1);
    } 
    else if (pid == 0) {
        // Child process
        close(pipefd[1]);
        close(fd);
        sieve(pipefd[0]);
    } 
    else {
        // Parent process
        close(pipefd[0]);
        while (read(fd, &n, sizeof(n)) == sizeof(n)) {
            if (n % p != 0) {
                write(pipefd[1], &n, sizeof(n));
            }
        }
        close(fd);
        close(pipefd[1]);
        wait(0);  // Wait for the child process to finish
    }
}

int main(int argc, char *argv[]) 
{
    int fd[2];
    int pid;
    
    if (pipe(fd) < 0) {
        fprintf(2, "primes: pipe error\n");
        exit(1);
    }
    
    pid = fork();
    if (pid < 0) {
        fprintf(2, "primes: fork error\n");
        exit(1);
    } 
    else if (pid == 0) {
        // Child process
        close(fd[1]);
        sieve(fd[0]);
    } 
    else {
        // Parent process
        close(fd[0]);
        for (int i = begin; i <= end; i++) {
            write(fd[1], &i, sizeof(i));
        }
        close(fd[1]);
        wait(0);  // Wait for the child process to finish
    }
    
    exit(0);
}