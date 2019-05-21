#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <netdb.h>
enum Status{
    HTTP,
    HTTPS,
    OTHER
};

class Parse{
    public:
        //判断是否为http网站
        static Status judge_is_http(std::string &url)
        {
            if(nullptr != strstr(url.c_str(),"https"))
                return HTTPS;
            else if(nullptr != strstr(url.c_str(),"http"))
                return HTTP;
            else
                return HTTP;
        }
        //获取FQDN:主机名和域名
        static std::string get_fqdn(std::string url)
        {
            std::string fqdn = url.substr(7);
            return fqdn;
        }
        //获取hostip地址
        static struct hostent* get_host_ip(std::string &fqdn)
        {
            struct hostent*tmp = gethostbyname(fqdn.c_str());
            return tmp;
        }
        //获取文件相对路径
        static std::string get_file_path(std::string fqdn)
        {
            return fqdn.substr(fqdn.find('/'));
        }
        //获取文件名
        static std::string get_file_name(std::string fqdn)
        {
            std::string tmp = fqdn.substr(fqdn.rfind('/')+1);
            return tmp;
        }
        //获取.*td文件
        static std::string get_file_name_td(std::string name)
        {
            std::string tmp = name.substr(0,name.find('.'));
            tmp += ".*td";
            return tmp;
        }
};
