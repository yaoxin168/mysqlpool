#include "MysqlConn.h"

MysqlConn::MysqlConn()
{
    m_conn = mysql_init(nullptr);
    mysql_set_character_set(m_conn, "utf8");
}

MysqlConn::~MysqlConn()
{
    if (m_conn != nullptr)
    {
        mysql_close(m_conn);
    }
    freeResult();
    
}

bool MysqlConn::connect(string user, string passwd, string dbName, string ip, unsigned short port)
{
    //c_str：string转char*
    MYSQL* ptr = mysql_real_connect(m_conn, ip.c_str(), user.c_str(), passwd.c_str(), dbName.c_str(), port, nullptr, 0);
    //成功，则返回的ptr和m_conn是一样的，失败，则返回nullptr
    return ptr != nullptr;

}

bool MysqlConn::update(string sql)
{
    //返回0成功，非0失败
    if(mysql_query(m_conn, sql.c_str()))
    {
        return false;
    }
    return true;
}

bool MysqlConn::query(string sql)
{
    freeResult();//清空上次的查询结果
    if(mysql_query(m_conn, sql.c_str()))
    {
        return false;
    }
    //保存结果集
    m_result = mysql_store_result(m_conn);
    return true;
}

bool MysqlConn::next()
{
    if(m_result != nullptr)//如果结果集为空，就没必要遍历了
    {
        m_row = mysql_fetch_row(m_result);
    }

    return false;
}

string MysqlConn::value(int index)
{
    int rowcount = mysql_num_fields(m_result);//当前记录中的字段数量

    if(index >= rowcount || index < 0)
    {
        return string();
    }
    char* val = m_row[index];//取出第index个字段
    unsigned long length = mysql_fetch_lengths(m_result)[index];//mysql_fetch_lengths返回当前记录里所有字段的值的长度
    //获取length是为了避免val中有提前出现的/0导致数据不全
    return string(val, length);//把val里length个字符转为string类型
}

bool MysqlConn::transaction()
{
    return mysql_autocommit(m_conn, 0);//0表示要手动提交事务
}

bool MysqlConn::commit()
{
    return mysql_commit(m_conn);//提交事务
}

bool MysqlConn::rollback()
{
    return mysql_rollback(m_conn);//事务回滚
}

void MysqlConn::freeResult()
{
    if(m_result)
    {
        mysql_free_result(m_result);//释放结果集
        m_result = nullptr;
    }
}
