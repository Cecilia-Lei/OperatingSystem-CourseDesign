#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int readline(char* buf, int max) 
{
  int i = 0;
  while ((i < max) && (read(0, buf + i, 1) > 0)) {
    if (buf[i] == '\n')
      break;
    i++;
  }
  buf[i] = 0;
  return i;
}

int main(int argc, char* argv[]) 
{
  // Check if the correct number of arguments is provided
  if (argc < 2) {
    fprintf(2, "usage: xargs command\n");
    exit(1);
  }
  else if (argc > MAXARG - 1) {// Check if number of arguments exceeds system limit
    fprintf(2, "xargs: too many arguments\n");
    exit(1);
  }

  char buf[1024];      // Buffer for reading input lines
  char* args[MAXARG];  // Command arguments
  args[0] = argv[1];   // First argument is the command to execute

  // Process each line from stdin
  while (readline(buf, sizeof(buf) / sizeof(buf[0])) > 0) {
    for (int i = 2; i < argc; i++) 
      args[i - 1] = argv[i];
    
    args[argc - 1] = buf;  // Add the input line as the last argument
    args[argc] = 0;        // Null terminate the argument list

    // Create child process to execute command
    if (fork() == 0) {
      exec(argv[1], args);  // Execute command with constructed arguments
      fprintf(2, "xargs: exec %s failed\n", argv[1]);
    }
    wait(0);  // Waits for child to complete
  }

  exit(0);
}
