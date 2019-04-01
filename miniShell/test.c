#include <stdio.h>
#include<sys/wait.h>
#include<unistd.h>
int main(void)
{
  int pid = fork();
  if(pid == 0)
  {
    //child
    int fd[2];
    pipe(fd);
    pid = fork();
    if(pid == 0)
    {
      //child_child
      close(fd[0]);
      dup2(fd[1],1);
      execlp("ps","ps","aux",NULL);
    }
    else if(pid>0)
    {
      //child_father
      // 
    }
      close(fd[1]);
      dup2(fd[0],0);
      execlp("grep","grep","test",NULL);
  }
  else if(pid>0)
  {
    //father
    wait(NULL);
  }
  return 0;
}

