#include <vector>
#include <pthread.h>
#include "pthreadPool.hpp"
class Singleton
{
    public:
        static Singleton* GetInstance()
        {
            if (nullptr == m_pInstance)
            {
                pthread_mutex_lock(&mutex);
                if (nullptr == m_pInstance)
                {
                    m_pInstance = new Singleton;
                }
                pthread_mutex_unlock(&mutex);
            }
            return m_pInstance;
        }

        //定义一个内部类。程序结束回收对象，因为是new申请了空间
        class cycle
        {
            public:
                ~cycle()
                {
                    if (m_pInstance != nullptr)
                    {
                        delete m_pInstance;
                        m_pInstance = nullptr;
                    }
                }
            private:
                //定义一个静态回收类对象
                static cycle endflag;
        };
        ~Singleton()
        {
            //pthread_mutex_destory(&mutex);
        }
        Singleton(const Singleton& s) = delete;
        Singleton operator=(Singleton& s) = delete;
    public:
        void thread_array_init(std::vector<thread_information> tmp)
        {
            thread_array.swap(tmp);
        }
        std::vector<thread_information> thread_array;
    private:
        Singleton(){
            pthread_mutex_init(&mutex,nullptr);
        }
        static Singleton *m_pInstance;//单例对象指针
        static pthread_mutex_t mutex;
};
Singleton* Singleton::m_pInstance = nullptr;
pthread_mutex_t Singleton::mutex;
Singleton::cycle Singleton::cycle::endflag;

