#include<iostream>
#include<thread>
#include<mutex>


//C++11之后线程安全的单例模式
//都可以使用
class single2{
    single2(){}
    single2(const single2&) = delete;
    single2& operator=(const single2&)=delete;
public:
    static single2& GetInst(){
        static single2 single;
        return single;
    }
};

void test_single2(){
    std::cout<<"s1 addr is "<<&single2::GetInst()<<std::endl;
    std::cout<<"s2 addr is "<<&single2::GetInst()<<std::endl;
}

//饿汉式初始化
class Single2Hungry
{
private:
    Single2Hungry()
    {
    }
    Single2Hungry(const Single2Hungry&) = delete;
    Single2Hungry& operator=(const Single2Hungry&) = delete;
public:
    static Single2Hungry* GetInst()
    {
        if (single == nullptr)
        {
            single = new Single2Hungry();
        }
        return single;
    }
private:
    static Single2Hungry* single;
};

//饿汉式初始化
Single2Hungry* Single2Hungry::single = Single2Hungry::GetInst();
void thread_func_s2(int i)
{
    std::cout << "this is thread " << i << std::endl;
    std::cout << "inst is " << Single2Hungry::GetInst() << std::endl;
}
void test_single2hungry()
{
    std::cout << "s1 addr is " << Single2Hungry::GetInst() << std::endl;
    std::cout << "s2 addr is " << Single2Hungry::GetInst() << std::endl;
    for (int i = 0; i < 3; i++)
    {
        std::thread tid(thread_func_s2, i);
        tid.join();
    }
}

//懒汉式初始化
class SinglePointer
{
private:
    SinglePointer()
    {
    }
    SinglePointer(const SinglePointer&) = delete;
    SinglePointer& operator=(const SinglePointer&) = delete;
public:
    static SinglePointer* GetInst()
    {
        if (single != nullptr)
        {
            return single;
        }
        s_mutex.lock();
        if (single != nullptr)
        {
            s_mutex.unlock();
            return single;
        }
        single = new SinglePointer();
        s_mutex.unlock();
        return single;
    }
private:
    static SinglePointer* single;
    static std::mutex s_mutex;
};

SinglePointer* SinglePointer::single = nullptr;
std::mutex SinglePointer::s_mutex;
void thread_func_lazy(int i)
{
    std::cout << "this is lazy thread " << i << std::endl;
    std::cout << "inst is " << SinglePointer::GetInst() << std::endl;
}
void test_singlelazy()
{
    for (int i = 0; i < 3; i++)
    {
        std::thread tid(thread_func_lazy, i);
        tid.join();
    }
    //何时释放new的对象？造成内存泄漏
}

//可以利用智能指针完成自动回收

class SingleAuto
{
private:
    SingleAuto()
    {
    }
    SingleAuto(const SingleAuto&) = delete;
    SingleAuto& operator=(const SingleAuto&) = delete;
public:
    ~SingleAuto()
    {
        std::cout << "single auto delete success " << std::endl;
    }
    static std::shared_ptr<SingleAuto> GetInst()
    {
        if (single != nullptr)
        {
            return single;
        }
        s_mutex.lock();
        if (single != nullptr)
        {
            s_mutex.unlock();
            return single;
        }
        single = std::shared_ptr<SingleAuto>(new SingleAuto);
        s_mutex.unlock();
        return single;
    }
private:
    static std::shared_ptr<SingleAuto> single;
    static std::mutex s_mutex;
};

std::shared_ptr<SingleAuto> SingleAuto::single = nullptr;
std::mutex SingleAuto::s_mutex;
void test_singleauto()
{
    auto sp1 = SingleAuto::GetInst();
    auto sp2 = SingleAuto::GetInst();
    std::cout << "sp1  is  " << sp1 << std::endl;
    std::cout << "sp2  is  " << sp2 << std::endl;
    //此时存在隐患，可以手动删除裸指针，造成崩溃
    // delete sp1.get();
}

//为了规避用户手动释放内存，可以提供一个辅助类帮忙回收内存
//并将单例类的析构函数写为私有

class SingleAutoSafe;
class SafeDeletor
{
public:
    void operator()(SingleAutoSafe* sf)
    {
        std::cout << "this is safe deleter operator()" << std::endl;
        delete sf;
    }
};
class SingleAutoSafe
{
private:
    SingleAutoSafe() {}
    ~SingleAutoSafe()
    {
        std::cout << "this is single auto safe deletor" << std::endl;
    }
    SingleAutoSafe(const SingleAutoSafe&) = delete;
    SingleAutoSafe& operator=(const SingleAutoSafe&) = delete;
    //定义友元类，通过友元类调用该类析构函数
    friend class SafeDeletor;
public:
    static std::shared_ptr<SingleAutoSafe> GetInst()
    {
        //1处
        if (single != nullptr)
        {
            return single;
        }
        s_mutex.lock();
        //2处
        if (single != nullptr)
        {
            s_mutex.unlock();
            return single;
        }
        //额外指定删除器  
        //3 处
        single = std::shared_ptr<SingleAutoSafe>(new SingleAutoSafe, SafeDeletor());
        //也可以指定删除函数
        // single = std::shared_ptr<SingleAutoSafe>(new SingleAutoSafe, SafeDelFunc);
        s_mutex.unlock();
        return single;
    }
private:
    static std::shared_ptr<SingleAutoSafe> single;
    static std::mutex s_mutex;
};

//call_once
class SingletonOnce {
private:
    SingletonOnce() = default;
    SingletonOnce(const SingletonOnce&) = delete;
    SingletonOnce& operator = (const SingletonOnce& st) = delete;
    static std::shared_ptr<SingletonOnce> _instance;
    
public:
    static std::shared_ptr<SingletonOnce> GetInstance() {
        static std::once_flag s_flag;
        std::call_once(s_flag, [&]() {
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
    
std::shared_ptr<SingletonOnce> SingletonOnce::_instance = nullptr;

void TestSingle() {
    std::thread t1([]() {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            SingletonOnce::GetInstance()->PrintAddress();    
        });

    std::thread t2([]() {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            SingletonOnce::GetInstance()->PrintAddress();
    });

    t1.join();
    t2.join();
}


//为了让单例更加通用，可以做成模板类
template <typename T>
class Singleton {
protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>& st) = delete;
    static std::shared_ptr<T> _instance;
public:
    static std::shared_ptr<T> GetInstance() {
        static std::once_flag s_flag;
        std::call_once(s_flag, [&]() {
            _instance = std::shared_ptr<T>(new T);
            });
        return _instance;
    }
    void PrintAddress() {
        std::cout << _instance.get() << std::endl;
    }
    ~Singleton() {
        std::cout << "this is singleton destruct" << std::endl;
    }
};
template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;

//想使用单例类，可以继承上面的模板，我们在网络编程中逻辑单例类用的就是这种方式
class LogicSystem :public Singleton<LogicSystem>
{
    friend class Singleton<LogicSystem>;
public:
    ~LogicSystem(){}
private:
    LogicSystem(){}
};

int main(){
    //test_single2();
    //test_single2hungry();
    //test_singlelazy();
}