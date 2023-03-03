#include <array>
#include <fstream>
#include <iostream>
#include <regex>
#include <set>
#include <string>
#include <variant>
#include <vector>
using namespace std;

int main (void){
    char *str = new char[64];
    std::string a="12345abcdefg6789";

    str[a.copy(str,7,5)]='\0';
    // 结果为 str="abcdefg";
    int lenth = a.copy(str,9);//a为源string copy 函数导入参数为被写入 str cp长度
    str[lenth]='\0';
    // 结果为 str="12345ab";
    cout<<lenth<<endl;
    cout<<str<<endl;
    cout<<a<<endl;
    delete[]str;

    /*注意
    *copy的第2，3个参数不能大于字符串str所能容纳的最长字符串长度
    */
    

}
