#include<iostream>
#include<thread>
#include<sstream>

void thread_work1(std::string str){
    std::cout<<"str is "<<str<<std::endl;
}

int main()
{
    std::string hellostr = "hello world !";
    //通过()初始化并启动一个线程
    std::thread t1(thread_work1,hellostr);
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    t1.join();

    return 0;
}