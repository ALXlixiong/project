#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__ 

#include "comm.hpp"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string>
class Client 
{
  private:
    int _sock;
    int _port;
    std::string _ip;
  public:
    Client(const int &port,const std::string ip)
      :_port(port)
      ,_ip(ip)
      ,_sock(-1)
  {}
    ~Client()
    {
      close(_sock);
    }
    void TwoDate(Request_t &rq)
    {
      Response_t rsq;
      std::cout<<"Please enter first date:";
      std::cin>>rq.d1;
      std::cin.clear();
      std::cin.sync();
      std::cin.ignore(1024,'\n');
      std::cout<<"Please enter second date:";
      std::cin>>rq.d2;
      send(_sock,&rq,sizeof(rq),0);
      recv(_sock,&rsq,sizeof(rsq),0);
      std::cout<<"result = "<<(int)rsq.result<<std::endl;
    }
    void DateDay(Request_t &rq)
    {
      Response_t rsq;
      std::cout<<"Please enter date:";
      std::cin>>rq.d1;
      std::cin.clear();
      std::cin.sync();
      std::cin.ignore(1024,'\n');
      std::cout<<"Please enter day:";
      std::cin>>rq.x;
      std::cin.clear();
      std::cin.sync();
      std::cin.ignore(1024,'\n');
      std::cout<<"Please enter op[+ -]>>";
      std::cin>>rq.op;
      send(_sock,&rq,sizeof(rq),0);
      recv(_sock,&rsq,sizeof(rsq),0);
      std::cout<<"result = "<<rsq.result_date<<std::endl;
    }
    void DateFun(Request_t &rq)
    {
      std::cout<<"--------------------------------"<<std::endl;
      std::cout<<"-------1:计算两个日期之差-------"<<std::endl;
      std::cout<<"-2:加上或者减去到指定日期的天数-"<<std::endl;
      int ch = 0;
      do{
begin:
        std::cout<<"请输入运算类型相应数字:";
        double kind = 0;
        std::cin>>kind;
        if(kind != 1.0 && kind != 2.0)
        {
          std::cout<<"Please enter 1 2 !!!"<<std::endl;
          std::cin.clear();
          std::cin.sync();
          std::cin.ignore(1024,'\n');
          goto begin;
        }
        int tmp = (int)kind;
        switch(tmp)
        {
          case 1:
            rq.y = 1;
            TwoDate(rq);
            break;
          case 2:
            rq.y = 2;
            DateDay(rq);
            break;
          default:
            std::cout<<"input error"<<std::endl;
            break;
        }
        std::cin.clear();
        std::cin.sync();
        std::cin.ignore(1024,'\n');
        std::cout<<"Whether or not to Continue[y/n]:";
        ch = getchar();
      }while(ch == 'y');
       
    }
    void NumberFun(Request_t &rq)
    {
      int ch = 0;
      do{
        Response_t rsq;
        std::cout<<"请输入操作数<x,y>";
        std::cin>>rq.x>>rq.y;
        std::cout<<"请输入操作码[+ - * / ^ %]"<<std::endl;
        std::cout<<"Please enter>>";
        std::cin>>rq.op;
        send(_sock,&rq,sizeof(rq),0);
        recv(_sock,&rsq,sizeof(rsq),0);
        if(rsq.status == 0)
          std::cout<<"result = "<<rsq.result<<std::endl;
        else if(rsq.status == -1)
          std::cout<<"input error"<<std::endl;
        std::cout<<std::endl;
        std::cout<<"Whether or not to Continue[y/n]:";
        std::cin.clear();
        std::cin.sync();
        std::cin.ignore(1024,'\n');
        ch = getchar();
      }while(ch == 'y');
    }
    void ExchangeFun(Request_t &rq)
    {
      int ch = 0;
      do
      {
begin:
        std::cout<<"Please enter decimal number:";
        std::cin>>rq.x;
        int tmp = (int)rq.x;
        if((tmp-rq.x) != 0)
        {
          std::cout<<"input error"<<std::endl;
          std::cin.clear();
          std::cin.sync();
          std::cin.ignore(1024,'\n');
          goto begin;
        }
        Response_t rsq;
        send(_sock,&rq,sizeof(rq),0);
        recv(_sock,&rsq,sizeof(rsq),0);
        bool flag = false;
        for(int i = 0;i<32;++i)
        {
          if(rsq.res[i] == 1 && flag == false)
            flag = true;
          if(flag == true)
            std::cout<<rsq.res[i];
        }
        if(flag == false)
          std::cout<<"0";
        std::cout<<std::endl;
        std::cout<<"Whether or not to Continue[y/n]:";
        std::cin.clear();
        std::cin.sync();
        std::cin.ignore(1024,'\n');
        ch = getchar();
      }while(ch == 'y');
    }
    void Run()
    {
      std::cout<<"--注意:输入操作数使用逗号间隔---"<<std::endl;
      for(;;)
      {
        Request_t rq;
        std::cout<<"-----请输入计算类型对应数字-----"<<std::endl;
        std::cout<<"--------------------------------"<<std::endl;
        std::cout<<"1:日期计算 2:数字计算 3:进制转化"<<std::endl;
        std::cout<<"--------------------------------"<<std::endl;
        std::cout<<"Please enter>>";
        std::cin>>rq.cal_kind;
        if(rq.cal_kind != 1.0 && rq.cal_kind != 2.0 && rq.cal_kind != 3.0)
        {
          std::cout<<"Please enter 1 2 3!!!"<<std::endl;
          std::cin.clear();
          std::cin.sync();
          std::cin.ignore(1024,'\n');
          continue;
        }
        int tmp = (int)rq.cal_kind;
        switch(tmp)
        {
          case 1:
            DateFun(rq);
            break;
          case 2:
            NumberFun(rq);
            break;
          case 3:
            ExchangeFun(rq);
            break;
          default:
            std::cerr<<"input error"<<std::endl;
            break;
        }
        std::cin.clear();
        std::cin.sync();
        std::cin.ignore(1024,'\n');
      }
    }
    void ClientInit()
    {
      _sock = socket(AF_INET,SOCK_STREAM,0);
      if(_sock<0)
      {
        std::cerr<<"client socket error"<<std::endl;
        exit(1);
      }
      struct sockaddr_in client;
      bzero(&client,sizeof(client));
      client.sin_family = AF_INET;
      client.sin_port = htons(_port);
      client.sin_addr.s_addr = inet_addr(_ip.c_str());
       
      int ret = connect(_sock,(struct sockaddr *)&client,sizeof(client));
      if(ret<0)
      {
        std::cerr<<"client connect error"<<std::endl;
        close(_sock);
        exit(1);
      }
      std::cout<<"connect success..."<<std::endl;
    }
};

#endif  //__CLIENT_HPP__
