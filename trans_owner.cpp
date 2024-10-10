#include<iostream>
#include<thread>

//转移线程所有权
void use_move()
{
    void some_function();
    void some_other_function();
    std::thread t1(some_function);
    std::thread t2 = std::move(t1);
    t1 = std::thread(some_other_function);
    std::thread t3;
    t3 = std::move(t2);
    t1 = std::move(t3);   //赋值操作将使程序崩溃
}

//离开其对象所在的作用域前，确保线程已经完结
class scoped_thread{
    std::thread t;
public:
    explicit scoped_thread(std::thread t_):t(std::move(t_)){
        if(!t.joinable())
            throw std::logic_error("NO Thread");
    }
    ~scoped_thread(){
        t.join();
    }
    scoped_thread(scoped_thread const&) = delete;
    scoped_thread& operator= (scoped_thread const&) = delete;
};
struct func
{
 int& i;
 func(int& i_) : i(i_) {}
 void operator() ()
    {
 for (unsigned j=0 ; j<1000000 ; ++j)
        {
    std::cout<<i<<std::endl; // 1 潜在访问隐患：空引用
        }
    }
};
void f(){
    void do_something_in_current_thread(); 
    int some_local_state;
    scoped_thread t{std::thread(func(some_local_state))};
    do_something_in_current_thread();
}

int main()
{

    return 0;
}