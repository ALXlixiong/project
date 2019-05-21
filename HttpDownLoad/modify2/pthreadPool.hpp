#pragma once 
#include <iostream>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "comm.hpp"
typedef void (*handle_t)(thread_information); 
class Task 
{
  private:
    thread_information _thread;
    handle_t _handle;
  public:
    Task(thread_information thread,handle_t handle)
      :_thread(thread)
      ,_handle(handle)
    {
        std::cout<<"chu shi hua\n";
    }
    
    void Run()
    {
        std::cout<<"run-------\n";
        _handle(_thread);
    }
};

class pthreadPool
{
  private:
    int _nums;//创建多少线程
    int _idle;//空闲的线程
    std::queue<Task> task_queue;
    pthread_cond_t _cond;
    pthread_mutex_t _mutex;
  public:
    pthreadPool(int nums = 10)
      :_nums(nums)
      ,_idle(nums)
  {
    pthread_cond_init(&_cond,NULL);
    pthread_mutex_init(&_mutex,NULL);
  }
    void lockTaskQueue()
    {
      pthread_mutex_lock(&_mutex);
    }
    void unlockTaskQueue()
    {
      pthread_mutex_unlock(&_mutex);
    }
    bool isTaskQueueEmpty()
    {
      return task_queue.size()>0?false:true;
    }
    Task PopTaskQueue()
    {
      Task tmp = task_queue.front();
      task_queue.pop();
      return tmp;
    }
    void idleTaskQueue()
    {
      ++_idle;
      pthread_cond_wait(&_cond,&_mutex);
      --_idle;
    }
    static void *pthread_route(void *arg)
    {
      pthread_detach(pthread_self());
      pthreadPool *p = (pthreadPool *)arg;
      for(;;)
      {
        p->lockTaskQueue();
        while(p->isTaskQueueEmpty())
          p->idleTaskQueue();
        Task t = p->PopTaskQueue();
        p->unlockTaskQueue();
        t.Run();
      }
      return NULL;
    }
    void WakeUpIdle()
    {
      pthread_cond_signal(&_cond);
    }
    void pushTask(Task t)
    {
      lockTaskQueue();
      task_queue.push(t);
      WakeUpIdle();
      unlockTaskQueue();
    }
    void pthreadPoolInit()
    {
      for(auto i = 0;i<_nums;++i)
      {
        pthread_t tid;
        pthread_create(&tid,NULL,pthread_route,(void *)this);
      }
    }

    ~pthreadPool()
    {
      pthread_cond_destroy(&_cond);
      pthread_mutex_destroy(&_mutex);
    }
};
