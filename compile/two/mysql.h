#include <iostream>
#include <string>
#include <mysql/mysql.h>
#include <stdlib.h>
#include <unistd.h>
#include "question.hpp"
#include <vector>

class MyDb {

public:
  MyDb()
  {
    mysql = mysql_init(NULL);
    if(!mysql) {
      std::cout<<"Error:"<<mysql_error(mysql);
      exit(1);
    }
  }
  ~MyDb()
  {
    if(mysql) {
      mysql_close(mysql);
    }
  }

    /**
     * 连接mysql
     *
     * @param host
     * @param user
     * @param pwd
     * @param db_name
     * @param port
     * @return
     */
  bool initDB(std::string host, std::string user,std::string passwd,
      std::string db_name, int port)
  {
    mysql = mysql_real_connect(mysql, host.c_str(), user.c_str(),
        passwd.c_str(), db_name.c_str(), port, NULL, 0);
    if(!mysql) {
      std::cout << "Error: " << mysql_error(mysql);
      exit(1);
    }

    return true;
  }
    /**
     * 执行sql语句
     *
     * @param sql
     * @return
     */
  bool exeSQL(std::string sql,std::vector<Question> &aq)
  {
    //mysql_query()执行成功返回0,执行失败返回非0值。
    if (mysql_query(mysql,sql.c_str())) {
      std::cout<<"Query Error: "<<mysql_error(mysql);
      return false;
    }

    result = mysql_store_result(mysql);

    if (result) {
      //获取结果集中总共的字段数，即列数
      int num_fields = mysql_num_fields(result);
      unsigned long long  num_rows = mysql_num_rows(result);
      aq.resize(num_rows);
      for(unsigned long long i = 0; i < num_rows; i++) {
        row = mysql_fetch_row(result);
        if(!row) {
          break;
        }
        Question q;
        std::vector<std::string> tmp;
        for(int j=0;j<num_fields;j++) {
          tmp.push_back(row[j]);
        }
        q.id = tmp[0];
        q.name = tmp[1];
        q.star = tmp[2];
        q.describe = tmp[3];
        q.test_case = tmp[4];
        q.code_structure = tmp[5];
        q.time_limit = tmp[6];
        q.mem_limit = tmp[7];
        aq.push_back(q);
      }
    } 
    else {
      //代表执行的是update,insert,delete类的非查询语句
      if (mysql_field_count(mysql) == 0) {
        // 返回update,insert,delete影响的行数
        unsigned long long num_rows = mysql_affected_rows(mysql);

        return num_rows;
      } else {
        std::cout << "Get result error: " << mysql_error(mysql);
        return false;
      }
    }
    return true;
  }
private:
    //连接mysql句柄指针
    MYSQL *mysql;
    //指向查询结果的指针
    MYSQL_RES *result;
     //按行返回的查询信息
    MYSQL_ROW row;
};
