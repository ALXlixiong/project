#include "comm.hpp"
#include "client.hpp"

static void Usage(std::string str)
{
  std::cout<<"Usage: "<<str<<" ip port"<<std::endl;
}
int main(int argc,char *argv[])
{
  if(argc != 3)
  {
    Usage(argv[0]);
    exit(1);
  }
  Client *c = new Client(atoi(argv[2]),argv[1]);
  c->ClientInit();
  c->Run();
  delete c;
  return 0;
}
