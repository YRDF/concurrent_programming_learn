#include<iostream>
#include<thread>
#include<sstream>

void change_param(int& param){
    param++;
}

void ref_oops(int some_param){
    std::cout<<"befor change ,param is "<<some_param<<std::endl;
    std::thread t2(change_param,std::ref(some_param));
    t2.join();
    std::cout<<"after change,param is "<<some_param<<std::endl;
}

int main()
{
    ref_oops(100);

    return 0;
}