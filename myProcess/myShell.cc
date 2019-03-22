#include <vector>
#include "util.hpp"

int main()
{  
  //scanf commond line 
  do 
  {
    //get user name 
    std::string info;
    GetUserName(info);
    //get local host name
    GetLocalHostName(info);
    //get current dir 
    GetCurrentDir(info);
    std::cout<<info;
    fflush(stdout);

    std::string comm_line;
    getline(std::cin,comm_line);
    //analyze command line parameter
    AnalyzeCommLine(comm_line);
    do_execute();
  }while(1);
  return 0;
}
