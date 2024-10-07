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

struct func {
    int& _i;
    func(int & i): _i(i){}
    void operator()() {
        for (int i = 0; i < 3; i++) {
            _i = i;
            std::cout << "_i is " << _i << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
};

void oops() {

        int some_local_state = 0;
        func myfunc(some_local_state);
        std::thread functhread(myfunc);
        //隐患，访问局部变量，局部变量可能会随着}结束而回收或随着主线程退出而回收
        functhread.detach();    
}
//join用法
void use_join(){
    int some_local_state = 0;
    func myfunc(some_local_state);
    std::thread functhread(myfunc);
    functhread.join();
}

//异常处理
void catch_exception() {
    int some_local_state = 0;
    func myfunc(some_local_state);
    std::thread  functhread{ myfunc };
    try {
        //本线程做一些事情,可能引发崩溃
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }catch (std::exception& e) {
        functhread.join();
        throw;
    }

    functhread.join();
}

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

    //detach 让子线程在后台运行，不会受主线程的影响 
    //线程允许采用分离的方式在后台独自运行，C++ concurrent programing书中称其为守护线程。
    // detach 注意事项
    oops();
    //防止主线程退出过快，需要停顿一下，让子线程跑起来detach
    //std::this_thread::sleep_for(std::chrono::seconds(1));

    //join 用法
    use_join();

    //捕获异常
    catch_exception();

    return 0;
}