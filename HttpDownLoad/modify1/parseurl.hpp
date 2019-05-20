#pragma once
#include <iostream>
#include <string>
#include <cstring>
enum Status{
    HTTP,
    HTTPS,
    OTHER
};

class Parse{
    public:
        //判断是否为http网站
        static Status JudgeIsHttp(std::string url)
        {
            if(nullptr == strstr(url.c_str(),"http"))
                return OTHER;
            else if(nullptr != strstr(url.c_str(),"https"))
                return HTTPS;
            else
                return HTTP;
        }
        //获取FQDN:主机名和域名
        static std::string getfqdn(std::string url)
        {
            std::string fqdn = url.substr(7);
            std::cout<<"fqdn:"<<fqdn<<std::endl;
            return fqdn;
        }
        //
};
