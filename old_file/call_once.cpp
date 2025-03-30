#include<iostream>
#include<mutex>
#include<thread>
#include<memory>
//使用c++11的std::call::once
//once_flag只会被初始化一次，当其他线程调用std::call_once，发现
//s_flag已经初始化过，就会不执行后面的操作。
class SingletonOnce{
private:
    SingletonOnce(){}
    SingletonOnce(const SingletonOnce&)=delete;
    SingletonOnce& operator =(const SingletonOnce&) = delete;
    static std::shared_ptr<SingletonOnce> _instance;
public:
    static std::shared_ptr<SingletonOnce> GetInst(){
        static std::once_flag s_flag;
        std::call_once(s_flag,[&](){
            _instance = std::shared_ptr<SingletonOnce>(new SingletonOnce);
        });
        return _instance;
    }
        void PrintAddress() {
        std::cout << _instance.get() << std::endl;
    }

    ~SingletonOnce() {
        std::cout << "this is singleton destruct" << std::endl;
    }
};
//测试call_once
void TestSingle() {

    std::thread t1([]() {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            SingletonOnce::GetInst()->PrintAddress();    
        });

    std::thread t2([]() {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            SingletonOnce::GetInst()->PrintAddress();
    });

    t1.join();
    t2.join();
}

//为了让单例更加通用，可以做成模板类
//想实现单例类的时候，就继承它，即可批量生成单例类

int main()
{
    //TestSingle();
    return 0;
}