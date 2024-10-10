#include<iostream>
#include<thread>
#include<mutex>

class Single2{
private:
    Single2(){}
    Single2(const Single2&) = delete;
    Single2& operator=(const Single2&) = delete;
public:
    static Single2& getInst()
    {
        static Single2 single;
        return single;
    }
};

//饿汉式单例类
class SingleHungry
{
private:
    SingleHungry(){}
    SingleHungry(const SingleHungry&) = delete;
    SingleHungry& operator=(const SingleHungry&) = delete;
public:
    static SingleHungry* GetInst()
    {
        if(single == nullptr)
        {
            single = new SingleHungry();
        }
        return single;
    }
private:
    static SingleHungry* single;
};

//饿汉式初始化
SingleHungry* SingleHungry::single = SingleHungry::GetInst();
void thread_func_s2(int i)
{
    std::cout << "this is thread " << i << std::endl;
    std::cout << "inst is " << SingleHungry::GetInst() << std::endl;
}
void test_single2hungry()
{
    std::cout << "s1 addr is " << SingleHungry::GetInst() << std::endl;
    std::cout << "s2 addr is " << SingleHungry::GetInst() << std::endl;
    for (int i = 0; i < 3; i++)
    {
        std::thread tid(thread_func_s2, i);
        tid.join();
    }
}

int main()
{
    test_single2hungry();
    return 0;
}