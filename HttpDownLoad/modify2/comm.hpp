#include <iostream>
#include <string>
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
        int _thread_id;
        int _thread_sock;
        std::string _fqdn;
        std::string _url;
        std::string _file_name_td;
        long int _begin;
        long int _end;
        long int _read_byte;
        long int _write_byte;
        thread_information(){}
}; 
