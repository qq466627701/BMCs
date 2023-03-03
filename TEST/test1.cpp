#include<iostream>
#include<memory>
using namespace std;

class Base{
    public:
    void func(){
        printf("this is a base func\n");
    };
};
class Derived:public Base{
    public:
    void func(){
        printf("this is a derived func\n");
    };
};
class Ball{
    public:
    Ball(){
        cout<<"Ball init"<<endl;
    }
    ~Ball(){
        cout<<"Ball destory"<<endl;
    }
    void func(){
        cout<<"Ball jump"<<endl;
    }
};
int main (void)
{
    auto p1 = make_unique<Ball>();
    p1->func();
    cout<<"end"<<endl;
    return 0;
}