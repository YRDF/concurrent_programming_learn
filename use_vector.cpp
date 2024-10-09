//使用emplace_back
#include<iostream>
#include<thread>
#include<vector>
void do_work(unsigned id)
{
    std::cout << "Hello from thread " << std::this_thread::get_id() 
    << std::endl;
}
void f()
{
 std::vector<std::thread> threads;
 for (unsigned i = 0; i < 20; ++i)
 {
 threads.emplace_back(do_work,i); // 产生线程
 } 
 for (auto& entry : threads) // 对每个线程调用 join()
 entry.join(); 
}
int main()
{
    f();
    return 0;
}