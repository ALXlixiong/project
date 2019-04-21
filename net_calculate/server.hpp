#ifndef __SERVER_HPP__
#define __SERVER_HPP__ 

#include "comm.hpp"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string>
#include "pthreadPool.hpp"
#define MAX_QUEUE 10
#define EXP 0.000000000001

class Server;
typedef struct info_t
{
  Server *p;
  int sock;
}info_t;
class Server
{
  private:
    int _port;
    int listen_sock;
    pthreadPool _pool;
  public:
    Server(int port)
      :_port(port)
      ,listen_sock(-1)
      ,_pool(5)
  {
    _pool.pthreadPoolInit();
  }
    ~Server()
    {
      close(listen_sock);
    }
  void ServerInit()
  {
    listen_sock = socket(AF_INET,SOCK_STREAM,0);
    if(listen_sock<0)
    {
      std::cerr<<"socket error"<<std::endl;
      exit(1);
    }
    struct sockaddr_in local;
    bzero(&local,sizeof(local));
    local.sin_family = AF_INET;
    local.sin_port = htons(_port);
    local.sin_addr.s_addr = htonl(INADDR_ANY);//绑定主机任意一个地址
    int opt = 1;
    if(setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))<0)
    {
      std::cerr<<"setsockopt error"<<std::endl;
      exit(1);
    }
    if(bind(listen_sock,(const struct sockaddr *)&local,sizeof(local))<0)
    {
      std::cerr<<"bind error"<<std::endl;
      close(listen_sock);
      exit(1);
    }
    if(listen(listen_sock,MAX_QUEUE)<0)
    {
      std::cerr<<"listen error"<<std::endl;
      close(listen_sock);
      exit(1);
    }
  }
  static void NumberFun(Request_t rq,int &sock)
  {
    Response_t rsq;
    rsq.status = 0;
    switch(rq.op)
    {
      case '+':
        rsq.result = rq.x + rq.y;
        break;
      case '-':
        rsq.result = rq.x - rq.y;
        break;
      case '*':
        rsq.result = rq.x * rq.y;
        break;
      case '/':
        if(rq.y == 0)
          rsq.status = -1;
        break;
      case '%':
        if(fabs(rq.x-(int)rq.x)<EXP && fabs(rq.y-(int)rq.y)<EXP)
          rsq.result = (int)rq.x % (int)rq.y;  //两个数必须是整数
        else 
          rsq.status = -1;
        break;
      case '^':
        if(fabs(rq.x-(int)rq.x)<EXP && fabs(rq.y-(int)rq.y)<EXP)
          rsq.result = (int)rq.x ^ (int)rq.y;
        else 
          rsq.status = -1;
        break;
      default:
        std::cout<<"input option error"<<std::endl;
        break;
    }
    send(sock,&rsq,sizeof(rsq),0);
  }
  static void DateFun(Request_t rq,int &sock)
  {
    Response_t rsq;
    int tmp = (int)rq.y;
    switch(tmp)
    {
      case 1:
        rsq.result = rq.d1-rq.d2;
        break;
      case 2:
        {
          switch(rq.op)
          {
            case '+':
              rsq.result_date = rq.d1+rq.x;
              break;
            case '-':
              rsq.result_date = rq.d1-rq.x;
              break;
          }
        }
        break;
    }
    send(sock,&rsq,sizeof(rsq),0);
  }
  static void ExchangeFun(Request_t rq,int sock)
  {
    Response_t rsq;
    int num = (int)rq.x;
    for(int i = 31;i>=0;--i)
    {
      if(num == 0)
        break;
      rsq.res[i] = num%2;
      num /= 2;
    }
    send(sock,&rsq,sizeof(rsq),0);
  }
  static void Kind(Request_t rq,int &sock)
  {
    int tmp = (int)rq.cal_kind;
    switch(tmp)
    {
      case 1:  //日期
        DateFun(rq,sock);
        break;
      case 2:  //数字
        NumberFun(rq,sock);
        break;
      case 3:  //进制转化
        ExchangeFun(rq,sock);
        break;
      default:
        std::cout<<"input error"<<std::endl;
        break;
    }
  }
  static void thread_route(int sock)
  { 
    while(1)
    {
      Request_t rq;
      int ret_recv = recv(sock,&rq,sizeof(rq),0);
      if(ret_recv<0)
      {
        std::cerr<<"recv error"<<std::endl;
        exit(2);
      }
      else if(ret_recv == 0){
        std::cout<<"Client is quit"<<std::endl;
        break;
      }
      else{
        Kind(rq,sock);  
      }
    }
  }
  void Run()
  {
    struct sockaddr_in client;
    for(;;)
    {
      socklen_t len = sizeof(client);
      int client_sock = accept(listen_sock,(struct sockaddr *)&client,&len);
      if(client_sock<0)
      {
        std::cerr<<"accept error"<<std::endl;
        continue;
      }
      std::cout<<"get a client..."<<std::endl;
      Task t(client_sock,Server::thread_route);
      _pool.pushTask(t);
    }
  }
};


#endif  //__SERVER_HPP__
