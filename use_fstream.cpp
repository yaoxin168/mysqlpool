#include <iostream>
#include <fstream>
using namespace std;
int main()
{
    //1.打开文件
    // fstream file1;
    // file1.open("dbconf.json",ios::binary|ios::in);

    //2.直接使用ifstream或ofstream的构造函数来打开文件
    //fstream的两个子类：ifstream(默认以输入方式打开文件)、ofstream
    ifstream fin("dbconf.json");
    if(!fin)
    {
        cout<<"x"<<endl;
    }

    //3.读写：直接使用 << >>
    // char buf[80];
    // while (!fin.eof())
    // {
    //     fin >> buf;
    //     cout << buf<<endl;
    // }
    // string str;
    // while (getline(fin,str))//这个能读出空格
    // {
    //     cout<<str<<endl;
    // }

    
    //3.获取文件大小
    fin.seekg(0, fin.end);//先移动到文件尾
    int fileSize = fin.tellg();//计算文件大小
    fin.seekg(0, fin.beg);//移动回起始位置

    //4.读取
    char* data = new char[fileSize];
    fin.read(data, fileSize);

    cout<<data<<endl;


    //4.关闭
    fin.close();


    return 0;
}