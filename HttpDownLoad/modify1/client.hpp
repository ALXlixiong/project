#include <stdint.h>
#include "parseurl.hpp"
#include <sys/sysinfo.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
class file_information
{
    public:
        std::string _file_path;
        std::string _file_name;
        std::string _file_name_td;
        long int _file_size;
        file_information(){}
};

class thread_information
{
    public:
        pthread_t _pid;
        long int _begin;
        long int _end;
        long int _read_byte;
        long int _write_byte;
        thread_information(){}
};

class Client
{
    private:
        int _sock;
        int _port;
        int _pthread_number;
        std::string _url;
        std::string _fqdn;
        thread_information _thread_bag;
        file_information _file_bag;
    public:
        
        Client(std::string url)
            :_url(url)
        {
            _sock = -1;
            _port = 80;
            _pthread_number = get_nprocs();//获取当前可使用CPU个数，以达到多线程最优
            //如果创建线程个数太多，反而会增大CPU调度压力
            std::string fqdn = Parse::get_fqdn(url);
            _fqdn = fqdn.substr(0,fqdn.find('/'));
            //bzero(&_thread_bag,sizeof(thread_information));
            //bzero(&_file_bag,sizeof(file_information));
            _file_bag._file_name = Parse::get_file_name(fqdn);
            _file_bag._file_path = Parse::get_file_path(fqdn);
            _file_bag._file_name_td = Parse::get_file_name_td(_file_bag._file_name);
        }
        ~Client()
        {
            close(_sock);
        }
        void parse_httphead(std::string http_request)
        {
            std::cout<<"recv request...\n";
            Writen(_sock,http_request,http_request.size());
            //获取响应
            std::string http_response = "";
            char ch[1];
            int index = 0;
            while(read(_sock,ch,1)>0){
                http_response += ch[0];
                if(index>4 && http_response[index-3] == '\r' && http_response[index-2] == '\n'
                        && http_response[index-1] == '\r' && http_response[index] == '\n')
                {
                    break;
                }
                ++index;
            }
            //分析响应状态码
            parse_status_code(http_response);
            char * pos = strstr(http_response.c_str(),"Content-length:");
            if(pos == nullptr){
                pos = strstr(http_response.c_str(),"Content-Length:");
                if(pos == nullptr){
                    pos = strstr(http_response.c_str(),"content-length:");
                    if(pos == nullptr){
                        pos = strstr(http_response.c_str(),"content-Length:");
                        if(pos == nullptr)
                        {
                            std::cerr<<"no content-length\n";
                            exit(-1);
                        }
                    }
                }
            }
            char *end = strstr(pos,"\r");
            *end = '\0';
            pos += 16;
            _file_bag._file_size = atol(pos);
            std::cout<<"file-size:"<<_file_bag._file_size<<std::endl;
        }
        void download_file()
        {

        }

        void client_run()
        {
            Status status = Parse::judge_is_http(_url);
            if(status == HTTPS){
                std::cout<<"this is a https protocal!!!"<<std::endl;
                exit(-1);
            }
            else if(status == OTHER){
                std::cout<<"input error, please input http protocal!!!\n";
                exit(-1);
            }
            _sock = socket(AF_INET,SOCK_STREAM,0);
            if(_sock<0){
                std::cerr<<"socet error\n";
                exit(-1);
            }
            struct sockaddr_in server;
            server.sin_family = AF_INET;
            struct hostent *host = Parse::get_host_ip(_fqdn);
            server.sin_addr.s_addr = *(int *)host->h_addr_list[0];
            server.sin_port = htons(_port);
            int conn_ret = connect(_sock,(struct sockaddr *)&server,sizeof(server));
            if(conn_ret<0){
                std::cerr<<"connect error\n";
                exit(-1);
            }
            std::cout<<"connect success server...\n";
            //组织HTTP请求头
            std::string http_request = "";
            http_request += "HEAD ";
            http_request += _url;
            http_request += " HTTP/1.1\r\nHost: ";
            http_request += _fqdn;
            http_request += "\r\nConnection: close\r\n\r\n";
            std::cout<<http_request<<std::endl;
            //
            parse_httphead(http_request);
        }
    private:
        void parse_status_code(std::string http_response)
        {
            std::string tmp = http_response;
            tmp = tmp.substr(tmp.find(' ')+1);
            tmp = tmp.substr(0,tmp.find(' '));
            if(tmp == "403"){
                std::cerr<<"forbidden\n";
                exit(-1);
            }
            else if(tmp == "400"){
                std::cerr<<"not found\n";
                exit(-1);
            }
        }
       // ssize_t readn(int sock,char* buf,int size)
       // {
       //     ssize_t read_ret;
       //     int left_  = size;
       //     char *tmp = buf;
       //     while(left_ > 0)
       //     {
       //         if((read_ret = read(sock,tmp,left_))<0){
       //             if(errno == EINTR){
       //                 read_ret = 0;
       //             }
       //             else{
       //                 return -1;
       //             }
       //         }
       //         else if(read_ret == 0){
       //             break;
       //         }
       //         left_ -= read_ret;
       //         tmp = tmp.substr(read_ret);
       //     }
       //     return size-left_;
       // }
       // ssize_t Readn(int sock,std::string str,int size)
       // {
       //     ssize_t n = 0;
       //     if((n = readn(sock,str,size))<0){
       //         std::cerr<<"readn error\n";
       //         exit(-1);
       //     }
       //     return n;
       // }

        ssize_t writen(int sock,std::string str,int size)
        {
            ssize_t write_ret;
            std::string tmp = str;
            ssize_t left_ = size;
            while(left_ > 0)
            {
                if((write_ret = write(sock,str.c_str(),left_))<=0){
                    if(write_ret<0 && errno == EINTR){
                        write_ret = 0;
                    }
                    else{
                        return -1;
                    }
                }
                
                left_ -= write_ret;
                str = str.substr(write_ret);
            }
            return size;
        }
        ssize_t Writen(int sock,std::string str,int size)
        {
            int n = 0;
            if((n == writen(sock,str,size))<0){
                std::cerr<<"writen error\n";
                exit(-1);
            }
            return n;
        }
};
