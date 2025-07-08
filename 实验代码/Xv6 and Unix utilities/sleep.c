#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
    // Check if the correct number of arguments is provided
    if (argc != 2) {
        fprintf(2, "Usage: sleep <time>\n");
    }
    
    // Convert the argument to an integer
    int time = atoi(*++argv);
    
    // Call the sleep system call
    if (sleep(time) != 0) {
        fprintf(2, "Error: sleep failed for unknown reason\n");
    }
    
    exit(0);
}