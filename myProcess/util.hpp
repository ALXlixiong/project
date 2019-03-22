#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#define MAX 1024
char *element[MAX];

void GetUserName(std::string &info)
{
  struct passwd *pw = getpwuid(getuid());
  if(pw != nullptr)
  {
    info += '<';
    info += pw->pw_name;
    info += '@';
  }
}
void GetLocalHostName(std::string &info)
{
  char *buf = new char[1024];
  memset(buf,0,1024);
  gethostname(buf,1023);
  char *cur = buf;
  while(*cur != '.'){
    info += *cur;
    ++cur;
  }
  info += ' ';
  delete[] buf;
}

void GetCurrentDir(std::string &info)
{
  char *buf = new char[1024];
  memset(buf,'0',1024);
  getcwd(buf,1023);
  char *cur = buf;
  while(*cur != '0')
  {
    info += *cur;
    ++cur;
  }
  info += '>';
  delete[] buf;
}
//是否为后台进程
void is_back()
{
  int index =0;
  while(strcmp(element[index],"&") != 0)
  {
    ++index;
    if(element[index] == nullptr)
      break;
  }
  if(element[index] == nullptr)
    return;
  element[index] = nullptr;
  int pid =0;
  if((pid = fork())<0){
    perror("& fork error");
    return;
  }else if(pid == 0)
  {
    if(fork() == 0){
      signal(SIGTSTP,SIG_DFL);//暂停进程
      execvp(element[0],element);
    }else{
      exit(0);
    }
    exit(0);
  }
  else{
    return;
  }
}

void AnalyzeCommLine(std::string &comm_line)
{
  char *buf = new char[comm_line.size()+1];
  memset(buf,0,comm_line.size()+1);
  for(size_t i = 0;i<comm_line.size();++i)
    buf[i] = comm_line[i];
  char *tmp = strtok(buf," ");
  int index = 0;
  while(tmp != nullptr)
  {
    element[index++] = tmp;
    tmp = strtok(nullptr," ");
  }
  element[index] = nullptr;
  //这里不可以释放buf，否则element里面没有数据了
}
void is_dup_add()
{
  int index = 0;
  while(element[index] != nullptr){
    if(strcmp(element[index],">>") == 0)
      break;
    ++index;
  }
  if(element[index] == nullptr)
  {
    return;
  }
  int fd = open(element[index+1],O_WRONLY|O_APPEND|O_CREAT,0666);
  if(fd<0){
    perror(">> fork error");
    exit(1);
  }
  pid_t pid = fork();
  if(pid>0){
    waitpid(pid,nullptr,0);
  }else if(pid == 0){
    dup2(fd,1);
    execlp(element[0],element[0],nullptr); 
  }else{
    perror(">>fork");
    exit(1);
  }
  close(fd);

}

void is_dup()
{
  int index = 0;
  while(element[index] != nullptr){
    if(strcmp(element[index],">") == 0)
      break;
    ++index;
  }
  if(element[index] == nullptr)
  {
    return;
  }
  int fd = open(element[index+1],O_WRONLY|O_CREAT,0666);
  if(fd<0){
    perror("> fork error");
    exit(1);
  }
  pid_t pid = fork();
  if(pid>0){
    waitpid(pid,nullptr,0);
  }else if(pid == 0){
    dup2(fd,1);
    execlp(element[0],element[0],nullptr); 
  }else{
    perror(">fork");
    exit(1);
  }
  close(fd);
}

void builtin()
{
  if(strcmp(element[0],"cd") == 0)
  {
    if(element[1] != nullptr)
    {
      if(chdir(element[1]) == -1)
      {
        perror("chdir");
        return;
      }
    }
  }
  else if(strcmp(element[0],"exit") == 0)
  {
    if(element[1] != nullptr)
    {
      std::cerr<<"input command error"<<std::endl;
      return;
    }
    exit(0);
  }
  else if(strcmp(element[0],"history") == 0)
  {
    if(element[1] != nullptr)
    {
      std::cerr<<"input command error"<<std::endl;
      return;
    } 
    char buff[1024] = {0};
    int fd = open("./../../../../.bash_history",O_RDONLY);
    if(fd<0)
    {
      perror("open");
      return;
    }
    while(1)
    {
      size_t rdSize = read(fd,buff,1023);
      if(rdSize == 0)
        break;
      std::cout<<buff;
      memset(buff,0,1024);
    }
  }
}
void do_execute()
{
  //解析命令是不是内建指令cd/exit/history
  builtin();
  //解析指令是否为重定向
  is_dup();
  is_dup_add();
  is_back();
  int pid = fork();
  if(pid<0){
    perror("fork");
    exit(1);
  }
  else if(pid == 0){
    execvp(element[0],element);
  }
  else{
    wait(NULL);
  }
}
