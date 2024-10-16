#include<iostream>
#include<future>
#include<chrono>
#include <thread>

// 定义一个异步任务
std::string fetchDataFromDB(std::string query) {
    // 模拟一个异步任务，比如从数据库中获取数据
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return "Data: " + query;
}

//async异步执行用法：future返回执行结果，结果使用.get返回。在这中间可以做其他的事。
//默认情况下，std::async使用std::launch::async | std::launch::deferred策略。
//这意味着任务可能异步执行，也可能延迟执行，具体取决于实现。
//需要注意的是，不同的编译器和操作系统可能会有不同的默认行为。
void use_async(){
    // 使用 std::async 异步调用 fetchDataFromDB
    std::future<std::string> resultFromDB = std::async(std::launch::async,fetchDataFromDB,"Data");
    // 在主线程中做其他事情
    std::cout<<"Doing something else..."<<std::endl;
    // 从 future 对象中获取数据
    std::string dbData = resultFromDB.get();
    std::cout<<dbData<<std::endl;
}

//std::packaged_task包装
int my_task() {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "my task run 5 s" << std::endl;
    return 42;
}

void use_package() {
    // 创建一个包装了任务的 std::packaged_task 对象  
    std::packaged_task<int()> task(my_task);

    // 获取与任务关联的 std::future 对象  
    std::future<int> result = task.get_future();

    // 在另一个线程上执行任务  
    std::thread t(std::move(task));
    t.detach(); // 将线程与主线程分离，以便主线程可以等待任务完成  

    // 等待任务完成并获取结果  
    int value = result.get();
    std::cout << "The result is: " << value << std::endl;

}

int main()
{
    //use_async();
    use_package();
    return 0;
}