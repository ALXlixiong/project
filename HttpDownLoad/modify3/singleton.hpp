#include <vector>
#include <pthread.h>
#include "pthreadPool.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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
        
        Singleton(const Singleton& s) = delete;
        Singleton operator=(Singleton& s) = delete;
        ~Singleton()
        {
            std::cout<<"----------------end---------------------\n";
            //如果存在seek_file文件，需要清空文件内容，保存新的断点位置
            if(access("./seek_file.txt",F_OK) == 0){
                remove("./seek_file.txt");
            }
            //有seek_file文件表示发生了断点下载
            int fd = open("./seek_file.txt",O_CREAT|O_WRONLY,0644);
            if(fd < 0){
                std::cerr<<"open seek_file error\n";
            }
            else
            {
                for(int i = 0;i<thread_array.size();++i)
                {
                    std::string tmp = "";
                    tmp += std::to_string(thread_array[i]._write_byte);
                    tmp += " ";
                    tmp += std::to_string(thread_array[i]._end);
                    tmp += "\n";
                    if(write(fd,tmp.c_str(),tmp.size()) != tmp.size())
                        std::cerr<<"write seek_file error\n";
                }
            }
        }

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

