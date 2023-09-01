#include <thread>
#include <iostream>
using namespace std;

class A
{
private:
    int m=3;
public:
    void fun2(int num)
    {
        cout<<"ss"<<num<<endl;
    }
};


void fun()
{
    cout<<"cc"<<endl;
}

int main()
{
    A a;
    thread t(&A::fun2, &a, 5);
    // thread t2(fun);//写成thread t2(&fun)也一样，因为函数名就是函数地址

    t.join();

    return 0;

}