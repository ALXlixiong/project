//这个文件实现项目在线编译+运行功能
#include <atomic>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <ctime>
#include <sys/resource.h>
#include "util.hpp"
#include <jsoncpp/json/json.h>

#define EXP 0.0000001
class Compiler 
{
  public:
  //涉及到的文件
  //1.源代码文件
    static std::string ScrPath(const std::string &name){
      return "./files/" + name + ".cpp";
    }
  //2.编译错误文件
    static std::string CompileErrorPath(const std::string &name){
      return "./files/" + name + ".Compile_error";
    }
  //3.标准输入文件
    static std::string StdinPath(const std::string &name){
      return "./files/" + name + ".stdin";
    }
  //4.标准输出文件
    static std::string StdoutPath(const std::string &name){
      return "./files/" + name + ".stdout";
    }
  //5.标准错误文件
    static std::string StderrPath(const std::string &name){
      return "./files/" + name + ".stderr";
    }
  //6.可执行程序文件
    static std::string ExePath(const std::string &name){
      return "./files/" + name + ".exe";
    }
  //可以使用stdin、stdout、stderr文件要通知可执行程序，来完成，所以本质上就是进程间通信
  //在此处我们使用了文件进程间通信
  //除此之外，文件通信方便我们后面进行调试
  public:
    //Json::Value是JSON的一个核心类，借助这个类可以实现序列化和反序列化
    static bool CompileAndRun(const Json::Value& req_json,Json::Value& resp_json,
        std::string &time_limit,std::string &mem_limit)
    {
      //1.根据请求对象生成一个源代码文件
      if(req_json["code"].empty())
      {
        resp_json["reason"] = "code empty";
        resp_json["error"] = 3;//其他错误
        LOG(ERROR)<<"code empty"<<std::endl;
        return false;
      }
      //req_json["code"]根据key取出value，value类型也是Json::Value类型
      //所以通过asString()转化为字符串
      const std::string& code = req_json["code"].asString();
      //标准输入req_json中
      const std::string& str_stdin = req_json["stdin"].asString();
      //把代码写到文件中
      std::string file_name = WriteCodeFile(code,str_stdin);
      
      //2.调用g++进行编译（fork + exec）
      //    编译-》生成一个可执行程序，如果编译错误，吧错误重定向到文件中
      bool ret = compile(file_name);
      if(ret == false){
        std::string reason;
        FileUtil::Read(CompileErrorPath(file_name),&reason);
        resp_json["reason"] = reason;
        resp_json["error"] = 1;//编译错误
        LOG(INFO)<<"compile error"<<std::endl;
        return false;
      }
      //3.执行可执行程序，标准输入记录到文件中，然后重定向给可执行程序。
      //    把标准输出和标准错误重定向到文件
      //运行出错，主要是遇到了信号
      bool memret = false;
      double time_result = 0;
      int signal = run(file_name,mem_limit,memret,time_result);
      if(signal != 0){
        std::string reason;
        resp_json["reason"] = std::to_string(signal) + " interrupt process";
        resp_json["error"] = 2;//运行错误
        LOG(INFO)<<"run error"<<std::endl;
        return false;
      }
      //运行时间限制
      if(time_limit != ""){
        double tmp = atof(time_limit.c_str()); 
        if((tmp-time_result)>EXP){
          std::string reason;
          resp_json["reason"] = "run out of time limit";
          resp_json["error"] = 2;//运行错误
          LOG(INFO)<<"out of limit"<<std::endl;
          return false;
        }
      }
      if(!memret){
        std::string reason;
        resp_json["reason"] = "out of run memory";
        resp_json["error"] = 2;//运行错误
        LOG(INFO)<<"out of run memory"<<std::endl;
        return false;
      }
      //4.把程序的最终结果返回（构造resp_json对象
      resp_json["reason"] = "";
      resp_json["error"] = 0;
      std::string str_stdout;
      FileUtil::Read(StdoutPath(file_name),&str_stdout);
      resp_json["stdout"] = str_stdout;
      std::string str_stderr;
      FileUtil::Read(StderrPath(file_name),&str_stderr);
      resp_json["stderr"] = str_stderr;
      LOG(INFO)<<"Process End"<<std::endl;
      return true;
    }
  private:
    static int run(const std::string& file_name,std::string &mem_limit,
        bool &runretlimit,double &time_result)
    {
      //1.创建子进程
      //2.父进程进行等待
      //3.子进程程序替换
      runretlimit = Limit(mem_limit);
      clock_t start = clock();
      pid_t pid = fork();
      if(pid>0){
        //father
        int status;
        waitpid(pid,&status,0);
        return status & 0x7f;
      }else if(pid == 0){
        //child
        //重定向(stdin,stdout,stderr)
        //标准输入还在req_json中
        int fd_stdin = open(StdinPath(file_name).c_str(),O_RDONLY,0666);
        if(fd_stdin<0)
        {
          LOG(ERROR)<<"run open stdinfile error"<<std::endl;
          exit(1);
        }
        dup2(fd_stdin,0);
        int fd_stdout = open(StdoutPath(file_name).c_str(),O_WRONLY|O_CREAT,0666);
        if(fd_stdout<0)
        {
          LOG(ERROR)<<"run open stdoutfile error"<<std::endl;
          exit(1);
        }
        dup2(fd_stdout,1);
        int fd_stderr = open(StderrPath(file_name).c_str(),O_WRONLY|O_CREAT,0666);
        if(fd_stderr<0)
        {
          LOG(ERROR)<<"run open stderrfile error"<<std::endl;
          exit(1);
        }
        dup2(fd_stderr,2);
        execlp(ExePath(file_name).c_str(),ExePath(file_name).c_str(),nullptr);
        exit(0);
      }
      else{
        //error
        LOG(ERROR)<<"run create fork error"<<std::endl;
        exit(1);
      }
      clock_t end = clock();
      time_result = (double)(end-start);
    }
    static std::string WriteCodeFile(const std::string& code,const std::string& str_stdin)
    {
      //1、把代码写到源文件
      //如：tmp_1234560819.1.cpp//在时间戳后面再加一个原子计数器
      //请求计数器：++不是原子操作
      //（1）加锁保证原子操作
      //（2）C++中atomic
      static std::atomic_int id(0);
      ++id;
      std::string file_name = "tmp_";
      file_name += std::to_string(TimeUtil::TimeStamp())+
        "." + std::to_string(id);
      FileUtil::Write(ScrPath(file_name).c_str(),code);
      FileUtil::Write(StdinPath(file_name).c_str(),str_stdin);
      //2、返回一个同一个文件名前缀
      return file_name;
    }
    static bool compile(const std::string& file_name)
    {
      //判断文件是否包含可以关键字，fork/system。。。
      if(!ProtectUtil::JudgeIsWord(ScrPath(file_name))){
        const std::string errorString = "code include fork/system...";
        FileUtil::Write(CompileErrorPath(file_name),errorString);
        return false;
      }
      //创建子进程
      //父进程等待
      //子进程程序替换
      pid_t ret = fork();
      if(ret>0){
        //父进程
        //必须阻塞等待
        waitpid(ret,nullptr,0);
      }else if(ret == 0){
        //子进程
        int fd = open(CompileErrorPath(file_name).c_str(),O_WRONLY|O_CREAT,0666);
        if(fd<0)
        {
          LOG(ERROR)<<"open CompilerFile error"<<std::endl;
          exit(1);
        }
        dup2(fd,2);
        execlp("g++","g++",ScrPath(file_name).c_str(),"-o",ExePath(file_name).c_str(),"-std=c++11",nullptr);
        exit(1);
      }else{
        //错误
        LOG(ERROR)<<"compile create fork error"<<std::endl;
        return false;
      }
      //到这里我们还不能够确定是否编译成功？
      //可以通过判断可执行文件是否存在，如果存在，那么编译成功，否则编译失败
      struct stat tv;
      int tv_ret =stat(ExePath(file_name).c_str(),&tv);
      if(tv_ret<0)
      {
        LOG(INFO)<<"compile error "<<file_name<<std::endl;
        return false;
      }
      LOG(INFO)<<"compile ok "<<file_name<<std::endl;
      return true;
    }
    static bool Limit(std::string &mem_limit)
    {
      struct rlimit memory_limit;

      //限制进程虚拟地址段大小
      memory_limit.rlim_cur = memory_limit.rlim_max = atoi(mem_limit.c_str());
      if(setrlimit(RLIMIT_AS, &memory_limit)){
        return false;
      }
      //限制进程数据段大小
      memory_limit.rlim_cur = memory_limit.rlim_max = atoi(mem_limit.c_str());
      if(setrlimit(RLIMIT_DATA, &memory_limit))
        return false;
      //限制进程栈大小
      memory_limit.rlim_cur = memory_limit.rlim_max = atoi(mem_limit.c_str()); 
      if(setrlimit(RLIMIT_STACK, &memory_limit))
        return false;
      return true;
    }
};
