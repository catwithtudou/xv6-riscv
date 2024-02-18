#include "kernel/types.h"
#include "user/user.h"

__attribute__((noreturn))
void deliver_process(int read_fd){
    int cur_num = 0;
    int is_fork = 0;
    int pass_num = 0;
    int pipes[2];
    while(1){
       // get a number from the left neighbor
       int read_len = read(read_fd,&pass_num,4);

       // the left neighbor empty or close
       if (read_len == 0 ){
           close(read_fd);
           if (is_fork){
               close(pipes[1]);
               close(pipes[0]);
               wait(0);
           }
           exit(0);
       }

       if (cur_num==0){
           cur_num = pass_num;
           printf("prime %d\n",cur_num);
       }

       if (pass_num%cur_num!=0) {
           if (!is_fork){
               // create new process and pipe
               pipe(pipes);
               is_fork = 1;
               int pid = fork();
               if (pid==0){ // child
                   // next process handle
                   // deliver to the next process pipes for writing
                   close(pipes[1]);
                   close(read_fd);
                   deliver_process(pipes[0]);
               } else{ // parent
                   close(pipes[0]);
               }
           }

           // send n to the right neighbor
           write(pipes[1],&pass_num,4);
       }
    }
}


int main(int argc, char *argv[]) {
    int pipes[2];
    pipe(pipes);
    for (int i=2;i<=35;i++) {
        write(pipes[1],&i,4);
    }
    close(pipes[1]);
    deliver_process(pipes[0]);
    exit(0);
}
