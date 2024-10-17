#include<iostream>
#include<functional>
#include<future>
#include<queue>

//利用c++的std::bind和伪闭包技术
//使得所有的函数都可以转化为void(void)的类型
//便于线程池调用。

//bind使int(int)函数变为int(void)
int functionint(int param) {
    std::cout << "param is " << param << std::endl;
    return 0;
}

void bindfunction() {
    std::function<int(void)> functionv = std::bind(functionint, 3);
    functionv();
}

//伪闭包lambda表达式使int(void)函数变为void(void)
//相当于插入队列了一个lambda表达式
void pushtasktoque() {
    std::function<int(void)> functionv = std::bind(functionint, 3);
    using Task = std::packaged_task<void()>;
    std::queue<Task> taskque;
    taskque.emplace([functionv]() {
        functionv();
        });
}

int main()
{
    pushtasktoque();
    return 0;
}