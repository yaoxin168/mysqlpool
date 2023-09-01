#pragma once
#include <string>
#include <mysql.h>
using namespace std;
class MysqlConn
{
public:
    //初始化数据库连接
    MysqlConn();//初始化数据库连接并设置字符集
    //释放数据库连接
    ~MysqlConn();
    //连接数据库
    bool connect(string user, string passwd, string dbName, string ip, unsigned short port = 3306);
    //更新数据库:insert, update, delete
    bool update(string sql);
    //查询数据库
    bool query(string sql);//查询得到的结果集保存在m_result中
    //遍历查询得到的结果集
    bool next();//遍历m_result
    //得到结果集中的字段值
    string value(int index);//取记录m_row里的字段值
    //事务操作
    bool transaction();//创建事务
    //提交事务
    bool commit();
    //事务回滚
    bool rollback();
private:
    void freeResult();//每次读完了结果集里的数据后，都需要释放指针m_result，清空结果集
    MYSQL* m_conn = nullptr;
    MYSQL_RES* m_result = nullptr;//结果集
    MYSQL_ROW m_row = nullptr;//MYSQL_ROW是一个指针数组，指向一条记录，记录里有许多字段。
                            //m_row指向的数组里的每个指针都是cha*类型，即字段是char*形式的
};