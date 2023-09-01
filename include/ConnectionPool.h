#include <queue>
#include "MysqlConn.h"
#include <mutex>
#include <condition_variable>
#include "cJSON.h"
using namespace std;

class ConnectionPool
{
public:
    static ConnectionPool* getConnectPool();
    shared_ptr<MysqlConn> getConnection();//提供给外部获取连接

private:
    ConnectionPool();
    ~ConnectionPool();
    ConnectionPool(const ConnectionPool& obj) = delete;
    ConnectionPool& operator=(const ConnectionPool& obj) = delete;

    bool paeseJsonFile();//解析json文件
    void produceConnection();//任务函数,生产数据库连接
    void recycleConnection();//任务函数,销毁数据库连接
    void addConnection();//调用MysqlConn的方法来连接数据库，然后把连接成功的对象conn保存到队列中

private:
    string m_ip;
    string m_user;
    string m_passwd;
    string m_dbName;
    unsigned short m_port;
    int m_minSize;
    int m_maxSize;
    int m_timeout;//超时时长(单位ms)，当连接池空了，线程阻塞会等待m_timeout后再尝试取连接,直到取到
    int m_maxIdleTime;//最大空闲时长(单位ms)，数据库连接达到这个空闲时间长度就释放掉
    mutex m_mutexQ;
    queue<MysqlConn*> m_connectionQ;

    condition_variable m_cond;
    
};