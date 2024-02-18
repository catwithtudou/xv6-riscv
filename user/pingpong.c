#include "kernel/types.h"
#include "user/user.h"

int main(){
  int pid,fork_pid;
  int child_pipe[2],parent_pipe[2];
  char buf[] = {'a'};

  pipe(child_pipe);
  pipe(parent_pipe);

  fork_pid = fork();

  if (fork_pid == 0) { // child
    pid = getpid();
    // step2: read a byte from the parent, and print
    close(parent_pipe[1]);
    read(parent_pipe[0],buf,1);
    printf("%d: received ping\n",pid);
    // step3: write a byte to the parent
    close(child_pipe[0]);
    write(child_pipe[1],buf,1);
  } else {  // parent
    pid = getpid();
    // step1: send a byte to the child
    close(parent_pipe[0]);
    write(parent_pipe[1],buf,1);
    // step4: read a byte from the child, and print
    close(child_pipe[1]);
    read(child_pipe[0],buf,1);
    printf("%d: received pong\n",pid);
  }

  exit(0);
}
