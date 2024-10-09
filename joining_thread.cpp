#include<iostream>
#include<thread>

class joining_thread
{
 std::thread t;
public:
 joining_thread() noexcept=default;
 template<typename Callable,typename ... Args>
 //callable是可调用对象（一个函数），args是函数的参数
 //这是一个模板接收多类型参数，通过原样转发（std::forward）的方法传入。
 explicit joining_thread(Callable&& func,Args&& ... args):
 t(std::forward<Callable>(func),std::forward<Args>(args)...)
 {}
 explicit joining_thread(std::thread t_) noexcept:
 t(std::move(t_))
 {}
 joining_thread(joining_thread&& other) noexcept:
 t(std::move(other.t))
 {}
 joining_thread& operator=(joining_thread&& other) noexcept
 {
//如果当前线程可汇合，则回合等待线程完成再赋值
 if(joinable()){
 join();
 }
 t = std::move(other.t);
 return *this;
 }
 joining_thread& operator=(std::thread other) noexcept
 {
 if(joinable())
 join();
 t=std::move(other);
 return *this;
 }
 ~joining_thread() noexcept
 {
 if(joinable())
 join();
 }
 void swap(joining_thread& other) noexcept
 {
 t.swap(other.t);
 }
 std::thread::id get_id() const noexcept{
 return t.get_id();
 }
 bool joinable() const noexcept
 {
 return t.joinable();
 }
 void join()
 {
 t.join();
 }
 void detach()
 {
 t.detach();
 }
 std::thread& as_thread() noexcept
 {
 return t;
 }
 const std::thread& as_thread() const noexcept
 {
 return t;
 }
};

void use_jointhread() {
    //1 根据线程构造函数构造joiningthread
    joining_thread j1([](int maxindex) {
        for (int i = 0; i < maxindex; i++) {
            std::cout << "in thread id " << std::this_thread::get_id()
                << " cur index is " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        }, 10);

    //2 根据thread构造joiningthread
    joining_thread j2(std::thread([](int maxindex) {
        for (int i = 0; i < maxindex; i++) {
            std::cout << "in thread id " << std::this_thread::get_id()
                << " cur index is " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        }, 10));

    //3 根据thread构造j3
    joining_thread j3(std::thread([](int maxindex) {
        for (int i = 0; i < maxindex; i++) {
            std::cout << "in thread id " << std::this_thread::get_id()
                << " cur index is " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        }, 10));


    //4 把j3赋值给j1，joining_thread内部会等待j1汇合结束后
    //再将j3赋值给j1
    j1 = std::move(j3);
}
int main()
{
    use_jointhread();

    return 0;
}