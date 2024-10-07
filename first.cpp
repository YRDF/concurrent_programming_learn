#include<iostream>
#include<thread>
#include<sstream>

void thread_work1(std::string str){
    std::cout<<"str is "<<str<<std::endl;
}
//当我们用仿函数作为参数传递给线程时，也可以达到启动线程执行某种操作的含义

class background_task {
public:
    void operator()(std::string str) {
        std::cout << "str is " << str << std::endl;
    }
};

int main()
{
    std::string hellostr = "hello world !";
    //通过()初始化并启动一个线程
    std::thread t1(thread_work1,hellostr);
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    t1.join();

    //如果采用如下方式启动函数，那一定会报错的。
    //std::thread t2(background_task());
    //t2.join();

    //可多加一层()
    std::thread t2((background_task()),"hello world");
    t2.join();

    //也可使用{}方式初始化
    std::thread t3{ background_task(),"hello world" };
    t3.join();

    //lambda表达式
    std::thread t4([](std::string str){
        std::cout << "str is " << str << std::endl;
    }, hellostr);
    t4.join();
    //join就是主线程等着子线程执行完了，主线程才能继续往下执行。

    //detach
    

    return 0;
}