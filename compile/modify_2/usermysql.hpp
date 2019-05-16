#include "mysql.h"
#include "util.hpp"

class UserMysql
{
  public:
    static bool JudgeUserOfMysql(std::unordered_map<std::string,std::string> &kv){
      std::vector<Question> aq;
      MyDb db;
      std::string host = "47.103.3.230";
      std::string user = "root";
      std::string passwd = "password";
      std::string dbName = "user";
      int port = 3306;
      //连接数据库
      bool conn = db.initDB(host, user, passwd, dbName, port);
      if (!conn) {
        LOG(INFO)<<"mysql connect fails\n";
      }
      std::string sql = "set names utf8";
      db.exeSQL(sql,aq);
      sql = "insert into userinfomation values";
      sql += "('"+kv["name"]+"'," +"'"+kv["phone"]+"',"+"'"+kv["email"]+"',"+"'"+kv["password"] + "');";
      if(!db.exeSQL(sql,aq)){
        LOG(INFO)<<"\nuser inromation insert error\n";
        //已经注册过了
        return false;
      }
      return true;
    }
    static bool JudgeIsLoginSuccess(std::unordered_map<std::string,std::string> &kv){
      MyDb db;
      std::string host = "47.103.3.230";
      std::string user = "root";
      std::string passwd = "password";
      std::string dbName = "user";
      int port = 3306;
      //连接数据库
      bool conn = db.initDB(host, user, passwd, dbName, port);
      if (!conn) {
        LOG(INFO)<<"mysql connect fails\n";
      }
      std::string sql = "set names utf8";
      sql = "select phone_number,email,passward from userinfomation where ";
      sql += "(phone_number=";
      sql += "'" + kv["phone"] + "' or ";
      sql += "email=";
      sql += "'" + kv["phone"] + "')";
      sql += "and passward=";
      sql += "'" + kv["Password"] + "';";
      if(!db.SelectResult(sql)){
          return false;
      }
      return true;
    }
};
