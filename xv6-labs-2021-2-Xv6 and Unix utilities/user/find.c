#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

// Extract the file name in the path
char* fmtname(char *path) 
{
  static char buf[DIRSIZ+1];
  char *p;
  
  // last '/'
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  
  // Copy the file name to the buffer
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  buf[strlen(p)] = 0;
  return buf;
}

void find(char *path, char *target) 
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  
  // Open the directory
  if((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
  
  // File information
  if(fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }
  
  // Path length
  if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
    fprintf(2, "find: path too long\n");
    close(fd);
    return;
  }
  
  // Process according to the file type
  switch(st.type) {
    case T_FILE:
      // file
      if(strcmp(fmtname(path), target) == 0) {
        printf("%s\n", path);
      }
      break;
    
    case T_DIR:
      // dir
      strcpy(buf, path);
      p = buf+strlen(buf);
      *p++ = '/';
      
      // Read each entry in the dir
      while(read(fd, &de, sizeof(de)) == sizeof(de)) {
        if(de.inum == 0)
          continue;
        
        // Skip the current dir and the superior dir
        if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
          continue;
        
        // Build and find sub-paths
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        find(buf, target);
      }
      break;
  }
  
  close(fd);
}

int main(int argc, char *argv[]) 
{
  // Check the number of parameters
  if(argc != 3) {
    fprintf(2, "Usage: find <path> <target>\n");
    exit(1);
  }

  find(argv[1], argv[2]);
  exit(0);
}