#include "comm.hpp"
#include "server.hpp"

void Usage(std::string str)
{
  std::cout<<"Usage: "<<str<<" port"<<std::endl;
}
int main(int argc,char *argv[])
{
  if(argc != 2)
  {
    Usage(argv[0]);
    exit(1);
  }
  Server *s = new Server(atoi(argv[1]));
  s->ServerInit();
  s->Run();
  delete s;
  return 0;
}
