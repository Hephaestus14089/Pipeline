#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>

/* Print error message and exit with status.  */
void panic(int status, const char * err_msg) {
  fprintf(stderr, "%s\n", err_msg);
  exit(status);
}

void pipeline(const char * cmd_a, const char * cmd_b) {
  int pipe_fd[2];

  if (pipe(pipe_fd) == -1) {
    panic(1, "Failed to open communication between file descriptors.");
  }

  pid_t pid = fork(); 

  if (pid == (pid_t) -1) {  // error
    panic(1, "Failed to create child process.");
  }
  else if (pid == (pid_t) 0) {  // child process
    close(pipe_fd[1]);  // close write end of pipe
    dup2(pipe_fd[0], STDIN_FILENO);  // redirect stdin to read end of pipe
    close(pipe_fd[0]);  // close read end of pipe

    execl("/bin/sh", "sh", "-c", cmd_b, (char *)NULL);
    // system(cmd_b);

    // panic(1, "Failed to execute command.");
    // panic(1, "Failed to execute command: \'%s\'", cmd_b);
  }
  else { //  parent process
    close(pipe_fd[0]);  // close read end of pipe
    dup2(pipe_fd[1], STDOUT_FILENO);  // redirect stdout to write end of pipe
    close(pipe_fd[1]);  // close write end of pipe

    // waitpid(pid, NULL, 0);  // wait for the child process to complete

    execl("/bin/sh", "sh", "-c", cmd_a, (char *)NULL);
    // system(cmd_a);

    // panic(1, "Failed to execute command.");
    // panic(1, "Failed to execute command: \'%s\'", cmd_a);
  }
}


int main(int argc, char **argv) {
  char * cmd_a = argv[1];
  char * cmd_b = argv[2];

  /* Testing */
  // char * cmd_a = "cat ~/.xinitrc";
  // char * cmd_b = "grep dwm";

  pipeline(cmd_a, cmd_b);

  return 0;
}
