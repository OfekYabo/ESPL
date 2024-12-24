#include "util.h"

#define SYS_WRITE 4
#define STDOUT 1
#define SYS_OPEN 5
#define O_RDWR 2
#define SYS_SEEK 19
#define SEEK_SET 0
#define SHIRA_OFFSET 0x291
#define SYS_EXIT 1

extern int system_call();

int main (int argc , char* argv[], char* envp[])
{
  /*Complete the task here*/
  // Loop through each argument
  for (int i = 0; i < argc; i++) {
      char *arg = argv[i];
      
      // Print each character of the argument
      while (*arg) {
          system_call(SYS_WRITE, STDOUT, arg, 1); // write(STDOUT, arg, 1)
          arg++;
      }
      
      // Print a newline after each argument
      system_call(SYS_WRITE, STDOUT, "\n", 1); // write(STDOUT, "\n", 1)
  }
  
  // Exit the program
  system_call(SYS_EXIT, 0); // exit(0)
  
  return 0; // This line will never be reached
}