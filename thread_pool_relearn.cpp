#include <atomic>
#include <condition_variable>
#include <future>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include<list>
#include<utility>

class NoneCopy{
public:
    ~NoneCopy(){}
protected:
    NoneCopy(){}
private:    
    NoneCopy(const NoneCopy&)=delete;
    NoneCopy& operator=(const NoneCopy&)=delete;
};

class ThreadPool:public NoneCopy{
public:
    //std::packaged_task打包函数
    using Task = std::packaged_task<void()>;
    ~ThreadPool(){
        stop();
    }

    static ThreadPool& instance(){
        static ThreadPool ins;
        return ins;
    }
    void start();
    void stop();
    //返回当前空闲线程数
    int idleThreadCount(){
        return thread_num_;
    }

    //处理往线程池投递的任务，转化为可接受的void (void)类型
   template <class F, class... Args>
auto commit(F&& f, Args&&... args) -> 
std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))> {
    using RetType = decltype(std::forward<F>(f)(std::forward<Args>(args)...));
    if (stop_.load())
        return std::future<RetType>{};

    auto task = std::make_shared<std::packaged_task<RetType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<RetType> ret = task->get_future();
    {
        std::lock_guard<std::mutex> cv_mt(cv_mt_);
        tasks_.emplace([task] { (*task)(); });
    }
    cv_lock_.notify_one();
    return ret;
}


private:
    ThreadPool();
    std::atomic_int             thread_num_;    //线程的数量
    std::queue<Task>            tasks_;         //任务队列
    std::vector<std::thread>    pool_;          //线程池
    std::atomic_bool            stop_;          //表示线程池是否退出(原子操作)

    std::mutex                  cv_mt_;         //互斥量 控制队列，往队列投入数据时加锁
    std::condition_variable     cv_lock_;       //配合线程的挂起和唤醒
};  
//线程池构造
ThreadPool::ThreadPool():stop_(false){
    unsigned int num = std::thread::hardware_concurrency();
    if(num<=1)
        thread_num_ = 2;
    else
        thread_num_ = num;
    //启动线程
    start();
}

//启动线程函数,把线程放入线程队列vector使用
void ThreadPool::start(){
    for(int i=0;i<thread_num_;++i){
        pool_.emplace_back([this](){
            while(!this->stop_.load()){
                Task task;
                {
                    std::unique_lock<std::mutex> cv_mt(cv_mt_);
                    this->cv_lock_.wait(cv_mt,[this](){
                        //条件为true继续，为false就在此等待 线程挂起
                        //线程池没停止，任务队列为空
                        return this->stop_.load()||this->tasks_.empty();
                    });
                    //要退出了或者队列不为空。cv_mt锁继续锁住
                    //如果任务队列为空，没任务就返回
                    if(this->tasks_.empty())
                        return;
                    //队列不为空
                    task = std::move(tasks_.front());
                    this->tasks_.pop();
                }
                    this->thread_num_--;
                    task();
                    this->thread_num_++;
            }
        });
    }
}

//stop_置为true，遍历vector等待所有线程结束
void ThreadPool::stop(){
    stop_.store(true);
    cv_lock_.notify_all();
    for(auto& td: pool_){
        if(td.joinable()){
            std::cout<<"join thread "<<td.get_id()<<std::endl;
            td.join();
        }
    }
}


template<typename T>
std::list<T>pool_thread_quick_sort(std::list<T> input) {
    if (input.empty())
    {
        return input;
    }
    std::list<T> result;
    result.splice(result.begin(), input, input.begin());
    T const& partition_val = *result.begin();
    typename std::list<T>::iterator divide_point =
        std::partition(input.begin(), input.end(),
            [&](T const& val) {return val < partition_val; });
    std::list<T> new_lower_chunk;
    new_lower_chunk.splice(new_lower_chunk.end(),
        input, input.begin(),
        divide_point);

    std::future<std::list<T>> new_lower = ThreadPool::instance().commit(pool_thread_quick_sort<T>, new_lower_chunk);

    std::list<T> new_higher(pool_thread_quick_sort(input));
    result.splice(result.end(), new_higher);
    result.splice(result.begin(), new_lower.get());
    return result;
}

void TestThreadPoolSort() {
    std::list<int> nlist = { 6,1,0,5,2,9,11 };
    std::cout<<"给出以下数字:"<<std::endl;
    std::cout<<" 6,1,0,5,2,9,11"<<std::endl;
    auto sortlist = pool_thread_quick_sort<int>(nlist);

    for (auto& value : sortlist) {
        std::cout << value << " ";
    }

    std::cout << std::endl;
}

int main()
{
    TestThreadPoolSort();
    return 0;
}