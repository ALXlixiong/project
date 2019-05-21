#include "parseurl.hpp"
#include "client.hpp"
int main()
{
    std::string url;
    std::cout<<"please input http_url>>";
    std::cin>>url;
    Client client(url);
    client.client_run();
}
