#include <iostream>
#include "ConnectionPool.h"
#include <memory>

int main()
{
    cout<<"ss"<<endl;

    ConnectionPool* pool= ConnectionPool::getConnectPool();
    cout<<"ss1"<<pool<<endl;
    shared_ptr<MysqlConn> ptr = pool->getConnection();
    cout<<"ss2"<<ptr<<endl;
    bool res = ptr->connect("yx","123456","scott", "172.20.28.250");
    cout << res<<endl;


    return 0;
}