#include<iostream>
#include<memory>

using namespace std;
 
class Base{
    public:
     virtual void func(){
        cout<<"this is base"<<endl;
    }
};
class Derived:public Base{
    public:
    virtual void func(){
        cout<<"this is derived"<<endl;
    }
    Derived();
};
int main (void){
    Base a;
    Derived b;
    Base *c=new Derived();
    c->func();
}