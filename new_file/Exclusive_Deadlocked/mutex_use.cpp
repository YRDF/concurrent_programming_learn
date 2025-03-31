//使用互斥量保证共享数据的安全
//死锁的相关处理方案
#include<iostream>
#include<mutex>
#include<thread>
#include<stack>

std::mutex mtx1;
int shared_data = 100;

void use_lock(){
    while(true){
        mtx1.lock();
        shared_data++;
        std::cout<<"current thread is "<<std::this_thread::get_id()<<std::endl;
        std::cout<<"share data is "<<shared_data<<std::endl;
        mtx1.unlock();
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
}

//锁的使用
void use_lockguard();
void test_lock(){
    std::thread t1(use_lockguard);
    std::thread t2([](){
        while(true){
            mtx1.lock();
            shared_data--;
            std::cout<<"current thread is "<<std::this_thread::get_id()<<std::endl;
            std::cout<<"share data is "<<shared_data<<std::endl;
            mtx1.unlock();
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    });
    t1.join();
    t2.join();
}

//lock_guard的raii思想
//lock_guard自动加锁和解锁
//lock_guard在作用域结束时自动调用其析构函数解锁
void use_lockguard() {
    while (true) {
        {
            std::lock_guard<std::mutex> lk_guard(mtx1);
            shared_data++;
            std::cout<<"current thread is "<<std::this_thread::get_id()<<std::endl;
            std::cout<<"share data is "<<shared_data<<std::endl;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
}


//如何保证数据安全
template<typename T>
class threadsafe_stack1{
private:
    std::stack<T> data;
    //mutable修改const
    mutable std::mutex m1;
public:
    threadsafe_stack1(){}
    threadsafe_stack1(const threadsafe_stack1& other){
        std::lock_guard<std::mutex> lock(other.m1);
        data = other.data;
    }

    threadsafe_stack1& operator=(const threadsafe_stack1&) = delete;

    void push(T new_value){
        std::lock_guard<std::mutex> lock(m1);
        data.push(std::move(new_value));
    }

    //问题代码
    T pop(){
        std::lock_guard<std::mutex> lock(m1);
        auto element = data.top();
        data.pop();
        return element;
    }
    //危险
    bool empty()const{
        std::lock_guard<std::mutex> lock(m1);
        return data.empty();
    }
};

void test_threadsafe_stacj1(){
    threadsafe_stack1<int> safe_stack;
    safe_stack.push(1);

    std::thread t1([&safe_stack](){
        if(safe_stack.empty()){
            safe_stack.pop();
        }
    });
 
    std::thread t2([&safe_stack](){
        if(safe_stack.empty()){
            safe_stack.pop();
        }
    });

    t1.join();
    t2.join();
}

struct empty_stack:std::exception{
    const char* what()const throw();
};
//线程安全的写法
template<typename T>
class threadsafe_stack{
    private:
        std::stack<T> data;
        //mutable修改const
        mutable std::mutex m1;
    public:
        threadsafe_stack(){}
        threadsafe_stack(const threadsafe_stack& other){
            std::lock_guard<std::mutex> lock(other.m1);
            data = other.data;
        }
    
        threadsafe_stack& operator=(const threadsafe_stack&) = delete;
    
        void push(T new_value){
            std::lock_guard<std::mutex> lock(m1);
            data.push(std::move(new_value));
        }
    
        //问题代码修改
        std::shared_ptr<T> pop(){
            std::lock_guard<std::mutex> lock(m1);
            if(data.empty()) {
                throw empty_stack();
            }
            std::shared_ptr<T> const res(std::make_shared<T>(data.top()));
            data.pop();
            return res;
        }
        //危险
        bool empty()const{
            std::lock_guard<std::mutex> lock(m1);
            return data.empty();
        }

        //第二种pop方式
        void pop(T& value){
            std::lock_guard<std::mutex> lock(m1);
            if(data.empty()){
                throw empty_stack();
            }
            value = data.top();
            data.pop();
        }
    };

//死锁
std::mutex t_lock1;
std::mutex t_lock2;
int m_1 = 0;
int m_2 = 1;

void dead_lock1(){
    while(true){
        std::cout<<"dead_lock1 begin"<<std::endl;
        t_lock1.lock();
        m_1 = 1024;
        t_lock2.lock();
        m_2 = 2048;
        t_lock2.unlock();
        t_lock1.unlock();
        std::cout<<"dead_lock1 end"<<std::endl;
    }
}

void dead_lock2(){
    while(true){
        std::cout<<"dead_lock2 begin"<<std::endl;
        t_lock2.lock();
        m_2 = 3072;
        t_lock1.lock();
        m_1 = 4096;
        t_lock1.unlock();
        t_lock2.unlock();
        std::cout<<"dead_lock2 end"<<std::endl;
    }
}

void test_dead_lock(){
    std::thread t1(dead_lock1);
    std::thread t2(dead_lock2);
    t1.join();
    t2.join();
}

//避免死锁的一个方式:将加锁和解锁的功能封装为独立的函数
//保证在独立的函数里执行完操作后就解锁，不会导致一个函数里使用多个锁的情况
//加锁和解锁作为原子操作解耦合，各自只管理自己的功能
void atomic_lock1() {
    std::cout << "lock1 begin lock" << std::endl;
    t_lock1.lock();
    m_1 = 1024;
    t_lock1.unlock();
    std::cout << "lock1 end lock" << std::endl;
}

void atomic_lock2() {
    std::cout << "lock2 begin lock" << std::endl;
    t_lock2.lock();
    m_2 = 2048;
    t_lock2.unlock();
    std::cout << "lock2 end lock" << std::endl;
}

void safe_lock1() {
    while (true) {
        atomic_lock1();
        atomic_lock2();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void safe_lock2() {
    while (true) {
        atomic_lock2();
        atomic_lock1();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void test_safe_lock() {
    std::thread t1(safe_lock1);
    std::thread t2(safe_lock2);
    t1.join();
    t2.join();
}

//同时加锁
//当我们无法避免在一个函数内部使用两个互斥量，并且都要解锁的情况
//那我们可以采取同时加锁的方式。
class some_big_object{
public:
    some_big_object(int data):_data(data){}
    //拷贝构造函数
    some_big_object(const some_big_object& b2):_data(b2._data){}
    
    //移动构造
    some_big_object(some_big_object&& b2):_data(std::move(b2._data)){}

    //重载赋值运算符
    some_big_object& operator=(const some_big_object& b2){
        _data = std::move(b2._data);
        return *this;
    }

    //重载移动赋值运算符
    some_big_object& operator=(const some_big_object&& b2){
        if(this == &b2){
            return *this;
        }
        _data = b2._data;
        return *this;
    }

    //重载输出运算符
    friend std::ostream& operator<<(std::ostream& os,const some_big_object& big_obj){
        os<<big_obj._data;
        return os;
    }

    //交换数据
    friend void swap(some_big_object& b1,some_big_object& b2){
        some_big_object temp = std::move(b1);
        b1 = std::move(b2);
        b2 = std::move(temp);
    }
private:
    int _data;
};

class big_object_mgr{
public:
    big_object_mgr(int data = 0) :_obj(data) {}
    void printinfo() {
        std::cout << "current obj data is " << _obj << std::endl;
    }
    friend void danger_swap(big_object_mgr& objm1, big_object_mgr& objm2);
    friend void safe_swap(big_object_mgr& objm1, big_object_mgr& objm2);
    friend void safe_swap_scope(big_object_mgr& objm1, big_object_mgr& objm2);
private:
    std::mutex _mtx;
    some_big_object _obj;
};

void danger_swap(big_object_mgr& objm1, big_object_mgr& objm2) {
    std::cout << "thread [ " << std::this_thread::get_id() << " ] begin" << std::endl;
    if (&objm1 == &objm2) {
        return;
    }
    std::lock_guard <std::mutex> gurad1(objm1._mtx);
    //此处为了故意制造死锁，我们让线程小睡一会
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::lock_guard<std::mutex> guard2(objm2._mtx);
    swap(objm1._obj, objm2._obj);
    std::cout << "thread [ " << std::this_thread::get_id() << " ] end" << std::endl;
}

void  test_danger_swap() {
    big_object_mgr objm1(5);
    big_object_mgr objm2(100);

    std::thread t1(danger_swap, std::ref(objm1), std::ref(objm2));
    std::thread t2(danger_swap, std::ref(objm2), std::ref(objm1));
    t1.join();
    t2.join();

    objm1.printinfo();
    objm2.printinfo();
}

void safe_swap(big_object_mgr& objm1, big_object_mgr& objm2) {
    std::cout << "thread [ " << std::this_thread::get_id() << " ] begin" << std::endl;
    if (&objm1 == &objm2) {
        return;
    }

    std::lock(objm1._mtx, objm2._mtx);
    //领养锁管理它自动释放
    std::lock_guard <std::mutex> gurad1(objm1._mtx, std::adopt_lock);

    //此处为了故意制造死锁，我们让线程小睡一会
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::lock_guard <std::mutex> gurad2(objm2._mtx, std::adopt_lock);

    swap(objm1._obj, objm2._obj);
    std::cout << "thread [ " << std::this_thread::get_id() << " ] end" << std::endl;
}

void test_safe_swap() {
    big_object_mgr objm1(5);
    big_object_mgr objm2(100);

    std::thread t1(safe_swap, std::ref(objm1), std::ref(objm2));
    std::thread t2(safe_swap, std::ref(objm2), std::ref(objm1));
    t1.join();
    t2.join();

    objm1.printinfo();
    objm2.printinfo();
}

//上述代码可以简化为以下方式
void safe_swap_scope(big_object_mgr& objm1, big_object_mgr& objm2) {
    std::cout << "thread [ " << std::this_thread::get_id() << " ] begin" << std::endl;
    if (&objm1 == &objm2) {
        return;
    }

    std::scoped_lock  guard(objm1._mtx, objm2._mtx);
    //等价于
    //std::scoped_lock<std::mutex, std::mutex> guard(objm1._mtx, objm2._mtx);
    swap(objm1._obj, objm2._obj);
    std::cout << "thread [ " << std::this_thread::get_id() << " ] end" << std::endl;
}


int main(){
    //test_lock();
    //test_threadsafe_stacj1();
    //test_dead_lock();
    //test_safe_lock();

    some_big_object bigobj1(100);
    some_big_object bigobj2(200);

    //bigobj1 = std::move(bigobj2);
    return 0;
}
