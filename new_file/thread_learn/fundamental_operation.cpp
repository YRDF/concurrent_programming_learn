#include<iostream>
#include<thread>

void thead_work1(std::string str) {
    std::cout << "str is " << str << std::endl;
}

class background_task{
public:
    void operator()() {
        std::cout<<"background_task is running!"<<std::endl;
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



int main() {
    std::string hellostr = "hello world!";
    //1 通过()初始化并启动一个线程
    std::thread t1(thead_work1, hellostr);
    
    //2 通过类对象初始化并启动一个线程  
    std::thread t2{background_task()}; // 通过类对象创建线程
    
    //3 通过lambda表达式初始化并启动一个线程
    std::thread t3([&hellostr](std::string str){
        std::cout << "str is " << str << std::endl;
    },hellostr);

    // // detach 注意事项
    // oops();
    // //防止主线程退出过快，需要停顿一下，让子线程跑起来detach
    // std::this_thread::sleep_for(std::chrono::seconds(1));



    t1.join(); // 等待线程结束
    t2.join(); // 等待线程结束
    t3.join(); // 等待线程结束
    std::cout << "main thread is running!" << std::endl;

    return 0;
}