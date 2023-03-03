#include <iostream>
using namespace std;
int main(void){
    uint16_t a;
    class A{
        int a = 1;
    };
    A a1;
    cout<<typeid(a).name()<<endl;
    std::cout<<typeid(A&).name()<<std::endl;
    return 0;
}