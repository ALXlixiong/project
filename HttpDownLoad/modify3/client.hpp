#include <stdint.h>
#include "parseurl.hpp"
#include <sys/sysinfo.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <vector>
#include <sys/stat.h>
#include <fcntl.h>
#include "pthreadPool.hpp"
#include "singleton.hpp"
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
        pthreadPool _pool;
    public:
        
        Client(std::string url)
            :_url(url)
        {
            _pool.pthreadPoolInit();
            _sock = -1;
            _port = 80;
            _pthread_number = 10;
            //_pthread_number = get_nprocs();//获取当前可使用CPU个数，以达到多线程最优
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
            download_file();
        }
        void download_file()
        {
            std::vector<thread_information> thread_array(_pthread_number);
            //如果*td不存在，则为一个新的下载
            long int avg_byte = 0;
            if(access("./seek_file.txt",F_OK) != 0){
                avg_byte = _file_bag._file_size/_pthread_number;
                long int start = 0;
                int i = 0;
                for(i = 0;i<_pthread_number-1;++i)
                {
                    thread_array[i]._thread_sock = -1;
                    thread_array[i]._read_byte = 0;
                    thread_array[i]._fqdn = _fqdn;
                    thread_array[i]._thread_id = i+1;
                    thread_array[i]._url = _url;
                    thread_array[i]._file_name_td = _file_bag._file_name_td;
                    thread_array[i]._write_byte = 0;
                    thread_array[i]._begin = start;
                    start += avg_byte;
                    thread_array[i]._end = start;
                    start += 1;
                }
                //最后一个线程需要特殊考虑，因为求字节平均值
                //会造成精度的损失，所以最后一个字节特殊考虑
                thread_array[i]._thread_sock = -1;
                thread_array[i]._read_byte = 0;
                thread_array[i]._fqdn = _fqdn;
                thread_array[i]._thread_id = i+1;
                thread_array[i]._url = _url;
                thread_array[i]._file_name_td = _file_bag._file_name_td;
                thread_array[i]._write_byte = 0;
                thread_array[i]._begin = start;
                thread_array[i]._end = _file_bag._file_size-1;
                //std::cout<<"---------------id:"<<i+1<<std::endl;
                //std::cout<<"---------------thread_array"<<thread_array[i]._begin<<std::endl;
                //std::cout<<"---------------thread_array"<<thread_array[i]._end<<std::endl;

                Singleton *m_instance = Singleton::GetInstance();
                m_instance->thread_array_init(thread_array);
                for(int i = 0;i<_pthread_number;++i)
                {
                    Task t(m_instance->thread_array[i],Client::pthread_route);
                    _pool.pushTask(t);
                }
            }
            else
            {
                int tdfd = open("./seek_file.txt",O_CREAT|O_WRONLY);
                if(tdfd < 0){
                    std::cerr<<"open seek file error\n";
                    exit(-1);
                }
            }
            sleep(5);
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
        static void pthread_route(thread_information thread_bag)
        {
            struct sockaddr_in server;
            server.sin_family = AF_INET;
            struct hostent *host = Parse::get_host_ip(thread_bag._fqdn);
            server.sin_addr.s_addr = *(int *)host->h_addr_list[0];
            server.sin_port = htons(80);
            thread_bag._thread_sock = socket(AF_INET,SOCK_STREAM,0);
            if(thread_bag._thread_sock<0){
                std::cerr<<"thread socket error\n";
                return;
            }
            if(connect(thread_bag._thread_sock,(struct sockaddr*)&server,sizeof(server))<0){
                std::cerr<<"thread connect error\n";
                return;
            }
            std::string http_request = "";
            http_request += "GET ";
            http_request += thread_bag._url;
            http_request += " HTTP/1.1\r\n";
            http_request += "Host: ";
            http_request += thread_bag._fqdn;
            http_request += "\r\nConnection: keep-alive\r\n";
            http_request += "Range: bytes=";
            http_request += std::to_string(thread_bag._begin);
            http_request += "-";
            http_request += std::to_string(thread_bag._end);
            http_request += "\r\n\r\n";
            int size = http_request.size();
            Writen(thread_bag._thread_sock,http_request,size);
            std::string http_response = "";
            char ch[1];
            int index = 0;
            while(read(thread_bag._thread_sock,ch,1)>0){
                http_response += ch[0];
                if(index>4 && http_response[index-3] == '\r' && http_response[index-2] == '\n'
                        && http_response[index-1] == '\r' && http_response[index] == '\n')
                {
                    break;
                }
                ++index;
            }
            parse_status_code(http_response);
            int file_fd = open(thread_bag._file_name_td.c_str(),O_CREAT|O_WRONLY,0644);
            if(file_fd < 0){
                std::cerr<<"open file error\n";
                return;
            }
            if(lseek(file_fd,thread_bag._begin,SEEK_SET)<0){
                std::cerr<<"lseek error\n";
                return;
            }
            long int div = thread_bag._end-thread_bag._begin+1;
            char buff[1024] = {0};
            ssize_t ret_read = 0;
            //std::cout<<"div:"<<div<<std::endl;
            while(thread_bag._write_byte != div)
            {
                memset(buff,0,sizeof(buff));
                int ret = 0;
                while(1)
                {
                    ret = recv(thread_bag._thread_sock,buff,1023,0);
                    if(ret >= 0)
                        break;
                    else if(ret<0 && (errno == EAGAIN||errno == EWOULDBLOCK||errno == EINTR))
                        continue;
                    else{
                        std::cerr<<"recv error\n";
                        exit(-1);
                    }
                }
                std::string tmp = "";
                for(int i = 0;i<ret;++i){
                    tmp += buff[i];
                }
                Writen(file_fd,tmp,ret);
                thread_bag._write_byte += ret;
                thread_bag._read_byte += ret;
            }
            close(thread_bag._thread_sock);
            close(file_fd);
        }
        long int get_write_in_file_byte(int fd)
        {
            struct stat st;
            if(fstat(fd,&st)<0){
                std::cerr<<"fstat error\n";
                exit(-1);
            }
            return st.st_size;
        }
        static void parse_status_code(std::string http_response)
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
        //static ssize_t readn(int sock,char(*buf)[1024],int size)
        //{
        //    ssize_t read_ret;
        //    int left_  = size;
        //    char *tmp = *buf;
        //    while(left_ > 0)
        //    {
        //        if((read_ret = read(sock,tmp,left_))<0){
        //            if(errno == EINTR){
        //                read_ret = 0;
        //            }
        //            else{
        //                return -1;
        //            }
        //        }
        //        else if(read_ret == 0){
        //            break;
        //        }
        //        left_ -= read_ret;
        //        tmp += read_ret;
        //    }
        //    return size-left_;
        //}
        //static ssize_t Readn(int sock,char(*str)[1024],int size)
        //{
        //    ssize_t n = 0;
        //    if((n = readn(sock,str,size))<0){
        //        std::cerr<<"readn error\n";
        //        exit(-1);
        //    }
        //    return n;
        //}

        static ssize_t writen(int sock,std::string str,int size)
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
        static ssize_t Writen(int sock,std::string str,int size)
        {
            int n = 0;
            if((n == writen(sock,str,size))<0){
                std::cerr<<"writen error\n";
                exit(-1);
            }
            return n;
        }
};
