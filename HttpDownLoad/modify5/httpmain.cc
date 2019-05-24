#include "parseurl.hpp"
#include "client.hpp"
#include <signal.h>
void argc(int sig)
{
    //std::cout<<"------------begin------------\n";
    //Singleton* m_instance = Singleton::GetInstance();
    //m_instance->~Singleton();
    Singleton::cycle _cycle;
    _cycle.~cycle();
    //std::cout<<"------------end------------\n";
    exit(0);
}
int main()
{
    signal(SIGINT,argc);
    std::string url;
    std::cout<<"please input http_url>>";
    std::cin>>url;
    Client client(url);
    client.client_run();
    return 0;
}
