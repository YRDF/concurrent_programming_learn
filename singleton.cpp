#include<iostream>
#include<thread>
#include<mutex>
#include<memory>

//有危险的单例类
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

//饿汉式单例类（使用之前就初始化了）
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

//饿汉式初始化和测试
SingleHungry* SingleHungry::single = SingleHungry::GetInst();
void thread_func_s2(int i)
{
    std::cout << "this is thread " << i << std::endl;
    //std::cout<<"this id is "<<std::this_thread::get_id()<<std::endl;
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

//懒汉式单例类（使用时才初始化,需要加锁）
//但是不知道是谁new的，所以怎么回收？有可能多重释放或者不知道哪个指针释放
class SinglePointer
{
private:
    SinglePointer(){}
    SinglePointer(const SinglePointer&) = delete;
    SinglePointer& operator = (const SinglePointer&) = delete;

public:
    static SinglePointer* GetInst()
    {
        if(single!= nullptr)
        {
            return single;
        }
        s_mutex.lock();
        if(single!=nullptr)
        {
            s_mutex.unlock();
            return single;
        }
        single = new SinglePointer();
        s_mutex.unlock();
        return single;        
    }
private:
    static SinglePointer *single;
    static std::mutex s_mutex;
};

//可以交给智能指针管理
class SingleAuto{
private:
    SingleAuto(){}
    SingleAuto(const SingleAuto&) = delete;
    SingleAuto & operator=(const SingleAuto&) = delete;
public:
    ~SingleAuto(){
        std::cout<<"single auto delete succes "<<std::endl;
    }

    static std::shared_ptr<SingleAuto> GetInst(){
        if(single != nullptr)
        {
            return single;
        }
        s_mutex.lock();
        if(single != nullptr)
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

//调用如下
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

int main()
{
    //test_single2hungry();
    test_singleauto();
    return 0;
}