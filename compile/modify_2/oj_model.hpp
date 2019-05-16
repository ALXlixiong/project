#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include "util.hpp"
#include "mysql.h"

//将题目加载到内存，通过哈希组织
//
struct cmp{
  bool operator()(std::string key1,std::string key2)const
  {
    return atoi(key1.c_str())<atoi(key2.c_str());
  }
};
class OJmodel{
    private:
        std::map<std::string,Question,cmp> _model;
    public:
        bool Load(std::string dbName){
          std::vector<Question> aq;
          MyDb db;
          std::string host = "47.103.3.230";
          std::string user = "root";
          std::string passwd = "password";
          int port = 3306;
          //连接数据库
          bool conn = db.initDB(host, user, passwd, dbName, port);
          if (!conn) {
            LOG(INFO)<<"mysql connect fails\n";
          }
          std::string sql = "set names utf8";
          db.exeSQL(sql,aq);
          sql = "select *from question";
          if(!db.exeSQL(sql,aq)){
            LOG(INFO)<<"get all question error\n";
            return false;
          }
          for(const auto &e:aq){
            _model[e.id] = e;
          }
          std::cout<<"load "<<_model.size()<<std::endl;
          return true;
        } 
        bool GetAllQuestion(std::vector<Question> &questions)const{
          questions.clear();
          for(const auto &e :_model){
            questions.push_back(e.second);
          }
          return true;
        }
        bool GetQuestion(std::string id,Question &q)const{
          auto pos = _model.find(id);
          if(pos != _model.end()){
            q = pos->second;
          }
          else
            return false;
          return true;
        }
};
