#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<pwd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

void do_parse(char buf[],char *argv[])
{
  //strtok是进行字符串切分
  char *ch = strtok(buf," ");
  int index = 0;
  while(ch != NULL)
  {
    argv[index++] = ch;
    ch = strtok(NULL," ");
  }
  argv[index] = NULL;
}

int IsPipe(char *argv[])
{
  int i = 0;
  while(argv[i] != NULL && strcmp(argv[i],"|")!= 0)
  {
    i++;
  }
  if(argv[i] == NULL){
    return -1;
  }
  else{
    return i;
  } 
}

void DivPipe(char *argv[],char *output[],char *input[],int div)
{
  int i = 0;
  while(i != div)
  {
    output[i] = argv[i];
    i++;
  }
  //printf("output:%s\n",output[0]);
  output[div] = NULL;
  i = 0;
  while(argv[div+i] != NULL)
  {
    input[i] = argv[div+i+1];
    i++;
  }
  input[i] = NULL;
}

void AchievePipe(char *output[],char *input[])
{
  int fd[2];
  pipe(fd);
  int pid = fork();
  if(pid == 0){
    //child
    close(fd[0]);
    dup2(fd[1],1);
    execvp(output[0],output);
    perror("fd[1]");
  }else if(pid>0){
    //father
    close(fd[1]);
    dup2(fd[0],0);
    execvp(input[0],input);
    perror("fd[0]");
  }else{
    perror("fork");
  }
  close(fd[1]);
  close(fd[0]);
}

//void AchievePipe(char *output[],char *input[])
//{
//  int fd[2];
//  pipe(fd);
//  int pid = fork();
//  if(pid == 0){
//    //child
//    if(fork() == 0){
//      close(fd[0]);
//      dup2(fd[1],1);
//      execvp(output[0],output);
//      perror("execvp1");
//    }
//    wait(NULL);
//    close(fd[1]);
//    dup2(fd[0],0);
//    execvp(input[0],input);
//    perror("execvp2");
//    exit(-1);
//    close(fd[0]);
//  }
//  else if(pid>0){
//    //father
//    wait(NULL);
//   // printf("test3\n");
//  }
//  else{
//    perror("fork");
//    exit(-1);
//  }
//  close(fd[0]);
//  close(fd[1]);
//}

void redup(char *argv[])
{
    int i = 0;
    for(;argv[i] != NULL;i++)
    {
      if(strcmp(argv[i],">") == 0)
      {
        FILE *scr_file = fopen(argv[1],"a+b");
        if(scr_file == NULL)
        {
          perror("scr_file");
          exit(-1);
        }
        //读取文件一的大小
        fseek(scr_file,0,SEEK_END);
        int file_size = ftell(scr_file);
        rewind(scr_file);
        
        char *buf = (char *)malloc(file_size);
        FILE *dst_file = fopen(argv[i+1],"w+b");
        if(dst_file == NULL)
        {
          perror("dst_file");
          fclose(scr_file);
          exit(-1);
        }
        fread(buf,1,file_size,scr_file);
        fwrite(buf,1,file_size,dst_file);
        fclose(scr_file);
        fclose(dst_file);
        exit(0);
      }
    }
}

void ChangeDir(char *argv[])
{
    char str[1024] = {0};
    if(strcmp(argv[0],"cd") == 0)
    {
      printf("str:%s\n",getcwd(str,sizeof(str)));
      if(-1 == chdir(argv[1]))
      {
        perror("chdir");
        exit(-1);
      }
      printf("str:%s\n",getcwd(str,sizeof(str)));
      exit(0);
    }
}
void do_create(char *argv[])
{  
  pid_t res = fork();
  if(res<0)
    perror("fork");
  else if(res == 0)
  {
    //切换目录
    ChangeDir(argv);
    //重定向
    redup(argv);
    //管道
    res =  IsPipe(argv);//res的位置是管道
    if(res>0)
    {
      //划分
      char *output[100] = {0};
      char *input[100] = {0};
      DivPipe(argv,output,input,res);
      //实现管道
      AchievePipe(output,input);
      //printf("test1\n");
      exit(-1);
    }
    //替换函数
    //一定是替换子进程，不然就回不去了
    int tmp = execvp(argv[0],argv);
    if(tmp<0)
    {
      perror("execvp");
      exit(-1);//替换失败之后，一定要退出，不然子进程会在后面循环执行父进程的代码，
    }
  }
  else 
  {
    while(1)
    {
      int pid = waitpid(-1,NULL,WNOHANG);
      if(pid<0)
        break;
    }
  }
}

void GetUserName()
{
  struct passwd *pw = getpwuid(getuid());
  if(pw != NULL)
  {
    printf("[%s@",pw->pw_name);
  }
}

void GetComputerName()
{
  char buf[1024];
  gethostname(buf,sizeof(buf)-1);
  char *cur = buf;
  while(*cur != '.')
    cur++;
  *cur = '\0';
  printf("%s ",buf);
}

void GetCurrentDir()
{
  char pwd[1024] = {0};
  getcwd(pwd,sizeof(pwd)-1);
  int len = strlen(pwd);
  char *cur = pwd+len-1;
  pwd[len] = '\0';
  while(*cur != '/')
  {
    cur--;
  }
  cur++;
  printf("%s]$",cur);
}
int main()
{
  while(1){
    //动态获取主机名、用户名和当前工作目录
    GetUserName();
    GetComputerName();
    GetCurrentDir();
    fflush(stdout);
    //1. 获取命令行
    char buf[100] = {0};
    scanf("%[^\n]%*c",buf);
    char *argv[100];
    //2. 解析命令行
    do_parse(buf,argv);
    //3. 创建子进程进行程序替换
    do_create(argv);
  }
  return 0;
}
