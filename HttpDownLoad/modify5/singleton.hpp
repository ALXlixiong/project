#include <vector>
#include <pthread.h>
#include "pthreadPool.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
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
            //std::cout<<"----------------singleton---------------------\n";
            //如果存在seek_file文件，需要清空文件内容，保存新的断点位置
            if(access("./seek_file.txt",F_OK) == 0){
                remove("./seek_file.txt");
            }
            //有seek_file文件表示发生了断点下载
            int fd = open("./seek_file.txt",O_CREAT|O_WRONLY,0644);
            if(fd < 0){
                std::cerr<<"open seek_file error\n";
                exit(-1);
            }
            for(int i = 0;i<thread_array.size();++i)
            {
                std::string path = "./id/"; 
                path += std::to_string(i+1);
                path += ".txt";
                std::string last_line = "";
                getLastLine(path,last_line);

                std::string tmp = "";
                tmp += last_line;
                tmp += " ";
                tmp += std::to_string(thread_array[i]._end);
                tmp += "\n";
                if(write(fd,tmp.c_str(),tmp.size()) != tmp.size()){
                    std::cerr<<"write seek_file error\n";
                }
            }
            std::cout<<"所有id文件都生成"<<std::endl;
            //for(int i = 0;i<thread_array.size();++i)
            //{
            //    std::string path = "./id/";
            //    path += std::to_string(i+1);
            //    path += ".txt";
            //    if(unlink(path.c_str()) != 0){
            //        std::cerr<<"unlink error\n";
            //    }
            //}
            std::cout<<"删除所有id文件"<<std::endl;
            //如果seek_file.txt最后一行的第一个数字等于文件的大小，那么说明文件已经下载完成
            //需要删除seek_file.txt,并且对文件重命名
            //std::string last_line = "";
            //getLastLine("./seek_file.txt",last_line);
            //std::string tmp = last_line.substr(0,last_line.find(' '));
            //if(tmp == std::to_string(thread_array[0]._file_size))
            //{
            //    std::cout<<"文件下载成功"<<std::endl;
            //    unlink("./seek_file.txt");
            //    rename(thread_array[0]._file_name_td.c_str(),thread_array[0]._file_name.c_str());
            //}
        }
        //void UpdateThreadArray(thread_information tmp)
        //{
        //    thread_array[tmp._thread_id-1]._write_byte = tmp._write_byte;
        //}
        //获取最后一行内容
        void getLastLine(std::string path,std::string &last_line)
        {
            std::string line;
            std::ifstream file(path.c_str());
            //if(file.is_open()){
            //    std::cerr<<"file is_open error\n";
            //}
            while(std::getline(file,line))
                last_line = line;
            file.close();
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

