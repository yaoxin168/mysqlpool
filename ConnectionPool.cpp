#include "ConnectionPool.h"
#include <fstream>
#include <iostream>
#include <thread>


ConnectionPool::ConnectionPool()
{
    //加载json配置文件
    if (!paeseJsonFile())
    {
        cout<<"配置文件加载失败"<<endl;
        return;
    }

    //初始化m_minSize个数据库连接
    for (int i = 0; i < m_minSize; ++i)
    {
        addConnection();
    }
    
    //创建2个线程用于 创建新连接,销毁多余连接
    thread producer(&ConnectionPool::produceConnection, this);
    thread recycler(&ConnectionPool::recycleConnection, this);

    //设置线程分离
    producer.detach();
    recycler.detach();

    
}

//在此析构掉队列中存储的MysqlConn*
ConnectionPool::~ConnectionPool()
{
    while (!m_connectionQ.empty())
    {
        MysqlConn* conn = m_connectionQ.front();
        m_connectionQ.pop();
        delete conn;
    }
    
}

ConnectionPool *ConnectionPool::getConnectPool()
{
    static ConnectionPool pool;
    return &pool;
}


/*
本来是返回一个MysqlConn*,但是使用完后需要还该连接,有两种还的方式
1.再写一个函数,使用者将这个连接通过参数传进行,该函数再将这个连接存储进数据库连接池的队列中

2.使用智能指针,当共享指针对应的对象析构时,它会先析构所管理的指针,
即shared_ptr<MysqlConn>会去释放掉MysqlConn*所指的连接.但我们的目的是将连接还回到连接池,而不是析构,
因此需要手动指定共享指针的删除器
*/
//消费者
shared_ptr<MysqlConn> ConnectionPool::getConnection()
{
    cout<<"getConnection"<<endl;
    unique_lock<mutex> locker(m_mutexQ);
    while (m_connectionQ.empty())
    {
        //阻塞指定时间长度后自动解除阻塞,或者是被生产线程者唤醒
        if(cv_status::timeout == m_cond.wait_for(locker, chrono::milliseconds(m_timeout)))
        {
        //如果是阻塞m_timeout毫秒导致的解除阻塞,则返回的状态是cv_status::timeout
            //这里为了保险起见,再判断一次是不是真的没拿到数据库连接
            if (m_connectionQ.empty())
            {
                continue;
            }   
        } 
    }
    cout<<"getConnection队列不空了"<<endl;
    //队列不空,可以取连接了
    shared_ptr<MysqlConn> connptr(m_connectionQ.front(), [this](MysqlConn* conn){
        //在此将连接还给数据库连接池队列
        lock_guard<mutex> locker(m_mutexQ);//m_connectionQ需要加锁
        conn->refreshAliveTime();
        m_connectionQ.push(conn);
    });//通过构造函数初始化共享指针
    m_connectionQ.pop();
    m_cond.notify_all();//唤醒生产者(这里由于生产者和消费者使用同一个条件变量来阻塞,其实也唤醒了消费者,但没事)
    return connptr;
}

bool ConnectionPool::paeseJsonFile()
{
    ifstream ifs("/home/yx/code/othercode/mysqlpool/dbconf.json");//以输入的方式打开文件
    if (!ifs)
    {
        cout<<"文件打开失败"<<endl;
        return false;
    }
    
    //获取文件大小
    ifs.seekg(0, ifs.end);
    int fileSize = ifs.tellg();
    //读取
    char* data = new char[fileSize];
    ifs.read(data, fileSize);
    //解析json
    cJSON* cjson_test = cJSON_Parse(data);
    //取键值对
    m_ip = cJSON_GetObjectItem(cjson_test, "ip")->valuestring;
    m_port = cJSON_GetObjectItem(cjson_test, "port")->valueint;
    m_user = cJSON_GetObjectItem(cjson_test, "userName")->valuestring;
    m_passwd = cJSON_GetObjectItem(cjson_test, "password")->valuestring;
    m_dbName = cJSON_GetObjectItem(cjson_test, "dbName")->valuestring;
    m_minSize = cJSON_GetObjectItem(cjson_test, "minSize")->valueint;
    m_maxSize = cJSON_GetObjectItem(cjson_test, "maxSize")->valueint;
    m_maxIdleTime = cJSON_GetObjectItem(cjson_test, "maxIdleTime")->valueint;
    m_timeout = cJSON_GetObjectItem(cjson_test, "timeout")->valueint;

    ifs.close();

    return true;
   
    
}
//生产者
void ConnectionPool::produceConnection()
{
    while (1)
    {
        unique_lock<mutex> locker(m_mutexQ);
        cout<<"创建线程"<<endl;
        m_cond.wait(locker, [this](){
            return m_connectionQ.size() < m_minSize;//比minSize少就不阻塞
        });//这里的写法相当于while(m_connectionQ.size() >= m_minSize){m_cond.wait(locker);};
        cout<<"正在创建线程"<<endl;
        //创建新的数据库连接
        addConnection(); 
        //唤醒消费者
        m_cond.notify_all();
    }
    
}

void ConnectionPool::recycleConnection()
{
    while (1)
    {
        this_thread::sleep_for(chrono::seconds(1));
        lock_guard<mutex> locker(m_mutexQ);
        while (m_connectionQ.size() > m_minSize)
        {
            MysqlConn* conn = m_connectionQ.front();
            if (conn->getAliveTime() >= m_maxIdleTime)//该连接的空闲时长 大于最大空闲时长,就销毁
            {
                m_connectionQ.pop();
                delete conn;
            }
            else{
                break;
            }
        }
    }
}

void ConnectionPool::addConnection()
{
    MysqlConn* conn = new MysqlConn;
    conn->connect(m_user, m_passwd, m_dbName, m_ip, m_port);//连接
    conn->refreshAliveTime();//记录起始的空闲时间点
    //把成功连接到mysql服务器的conn保存到队列中
    m_connectionQ.push(conn);
}
