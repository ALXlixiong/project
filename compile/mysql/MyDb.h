/**
 * file MyDb.h
 *
 * Created by shiyibo on 2018/12/21.
 */

#ifndef DB_MYDB_H
#define DB_MYDB_H

#include <string>
#include <mysql/mysql.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

class MyDb {

public:
    MyDb();
    ~MyDb();

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
    bool initDB(string host, string user,string pwd,string db_name, int port);

    /**
     * 执行sql语句
     *
     * @param sql
     * @return
     */
    bool exeSQL(string sql);

private:
    /**
     * 连接mysql句柄指针
     */
    MYSQL *mysql;

    /**
     * 指向查询结果的指针
     */
    MYSQL_RES *result;

    /**
     * 按行返回的查询信息
     */
    MYSQL_ROW row;
};
#endif //DB_MYDB_H
