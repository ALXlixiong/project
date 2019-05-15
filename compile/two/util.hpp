#pragma once 
#include <stdint.h>
#include <sys/time.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <boost/algorithm/string.hpp>
//获取时间戳工具
class TimeUtil
{
  public:
    static int64_t TimeStamp()
    {
      struct timeval tv;
      ::gettimeofday(&tv,nullptr);
      return tv.tv_sec;
    }
    static int64_t TimeStampMS()
    {
      struct timeval tv;
      ::gettimeofday(&tv,nullptr);
      return tv.tv_sec*1000 + tv.tv_usec/1000;
    }
};
//打印日志工具
//日志格式：
//[W1553052489 filename:line] hello
//日志使用格式
//LOG(WARNING)<<"hello"<<endl;
//日志级别：
//FATAL  致命
//ERROR  错误
//WARNING警告
//INFO   提示

enum Leval
{
  FATAL,
  ERROR,
  WARNING,
  INFO 
};

inline std::ostream& Log(enum Leval level,const std::string& file_name,int line_num)
{
  std::string tmp = "[";
  if(level == FATAL)
    tmp += "F";
  else if(level == ERROR)
    tmp += "E";
  else if(level == WARNING)
    tmp += "W";
  else if(level == INFO)
    tmp += "I";
  tmp += std::to_string(TimeUtil::TimeStamp());
  tmp += " ";
  tmp += file_name;
  tmp += ":";
  tmp += std::to_string(line_num);
  tmp += "] ";
  std::cout<<tmp;
  return std::cout;
}
#define LOG(level) Log(level,__FILE__,__LINE__)

//文件相关工具
class FileUtil
{
  public:
    static bool Read(const std::string& file_path,std::string *content)
    {
      content->clear();
      std::fstream file(file_path.c_str());
      if(!file.is_open()){
        return false;
      }
      std::string line = "";
      //getline默认读取不携带\n
      while(std::getline(file,line))
      {
        *content += line + '\n';
        line = "";
      }
      file.close();
      return true;
    }
    static bool Write(const std::string &file_path,const std::string& content)
    {
      std::ofstream file(file_path.c_str());
      if(!file.is_open()){
        return false;
      }
      file.write(content.c_str(),content.size());
      file.close();
      return true;
    }
};
//切割字符串工具
class StringUtil
{
  public:
    static void Split(std::vector<std::string>& output,const std::string& input,const  std::string& sp_string)
    {
      boost::split(output,input,boost::is_any_of(sp_string),boost::token_compress_on);
    }
};
//URL解析
class UrlUtil
{
  public:
    static void EncodeBody(const std::string& body,std::unordered_map<std::string,std::string>& kv)
    {
      //根据请求获取JSON结构
      std::vector<std::string> output_kv;
      StringUtil::Split(output_kv,body,"&");
      for(auto& e:output_kv)
      {
        std::vector<std::string> output;
        StringUtil::Split(output,e,"=");
        if(output.size() != 2){
          continue;
        }
        //unordered_map,如果key不存在就插入
        kv[output[0]] = UrlDecode(output[1]);
      }
      //urldecode
      //
    }
  private:
     static unsigned char ToHex(unsigned char x)   
    {   
      return  x > 9 ? x + 55 : x + 48;   
    }  

    static unsigned char FromHex(unsigned char x)   
    {   
      unsigned char y;  
      if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;  
      else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;  
      else if ((x >= '0' )&& (x <= '9')) y = x - '0';  
      else assert(0);  
      return y;  
    }  

    static std::string UrlEncode(const std::string& str)  
    {  
      std::string strTemp = "";  
      size_t length = str.length();  
      for (size_t i = 0; i < length; i++)  
      {  
        if (isalnum((unsigned char)str[i]) ||   
            (str[i] == '-') ||  
            (str[i] == '_') ||   
            (str[i] == '.') ||   
            (str[i] == '~'))  
          strTemp += str[i];  
        else if (str[i] == ' ')  
          strTemp += "+";  
        else  
        {  
          strTemp += '%';  
          strTemp += ToHex((unsigned char)str[i] >> 4);  
          strTemp += ToHex((unsigned char)str[i] % 16);  
        }  
      }  
      return strTemp;  
    }  

    static std::string UrlDecode(const std::string& str)  
    {  
      std::string strTemp = "";  
      size_t length = str.length();  
      for (size_t i = 0; i < length; i++)  
      {  
        if (str[i] == '+') strTemp += ' ';  
        else if (str[i] == '%')  
        {  
          assert(i + 2 < length);  
          unsigned char high = FromHex((unsigned char)str[++i]);  
          unsigned char low = FromHex((unsigned char)str[++i]);  
          strTemp += high*16 + low;  
        }  
        else strTemp += str[i];  
      }  
      return strTemp;  
    }  
};

//黑名单保护机制
//判断源文件内部是否存在特殊关键词如fork、system等
class ProtectUtil
{
  public:
    static bool JudgeIsWord(const std::string& file_name)
    {
      std::string content = "";
      FileUtil::Read(file_name,&content);
      std::cout<<content<<std::endl;
      if(content.find("fork()") != std::string::npos || content.find("system") != std::string::npos)
      {
        LOG(ERROR)<<"source file include fork/system..."<<std::endl;
        return false;
      }
      std::cout<<"no include"<<std::endl;
      return true;
    }
};
