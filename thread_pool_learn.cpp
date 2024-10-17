#include <atomic>
#include <condition_variable>
#include <future>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class NoneCopy{
public:
    ~NoneCopy(){}
protected:
    NoneCopy(){}
private:
    NoneCopy(const NoneCopy&) = delete;
    NoneCopy& operator= (const NoneCopy&) = delete;
};

class ThreadPool : public NoneCopy{
public:
    using Task = std::packaged_task<void()>;
    ~ThreadPool(){
        stop();
    }

    static ThreadPool& instance(){
        static ThreadPool ins;
        return ins;
    }

    //往线程池投递任务
    //因为我们要投入的参数和返回值不同，
    //而我们之前设置的线程池执行的task类型为void(void)。
    //那么怎么转换成void(void)呢？？？
    //使用std::bind进行绑定，再加工一层。可以使参数为void
    //使用伪闭包技术，让返回值为void。lambda表达式
    //利用lambda表达式生成一个返回值和参数都为void的函数
    //函数内部调用functionv即可
    //投递完了，判断是否执行完
    template<class F,class ... Args>
    auto commit(F&& f,Args&&... args)->std::future<decltype(f(args...))>{
        using RetType = decltype(f(args...));
        if(stop_.load())
            return std::future<RetType>{};
        
        auto task = std::make_shared<std::packaged_task<RetType()>>(
            std::bind(std::forward<F>(f),std::forward<Args>(args)...));

        std::future<RetType> ret = task->get_future();
        {
            std::lock_guard<std::mutex> cv_mt(cv_mt_);
            tasks_.emplace([task]{(*task)();});
        }
        cv_lock_.notify_one();
        return ret;
    }


   //查询当前线程数
   int idleThreadCount(){
    return thread_num_;
   }


private:
    //hardware_concurrency显式当前设备是几核的    
    ThreadPool(unsigned int num = std::thread::hardware_concurrency())
        :stop_(false){

            if(num<=1)
                thread_num_ = 2;
            else
                thread_num_ = num;
            start();
            
    }

    //启动线程池
    void start(){
        for(int i = 0;i<thread_num_;++i){
            pool_.emplace_back([this](){
                while(!this->stop_.load()){
                    Task task;
                    {
                        //局部作用域结束unique_lock被回收
                        std::unique_lock<std::mutex> cv_mt(cv_mt_);
                        //使用条件变量判断如果为true就继续，false就挂起等待
                        //true:stop为true或队列为空
                        this->cv_lock_.wait(cv_mt,[this]{
                            return this->stop_.load() || !this->tasks_.empty();
                        }); 
                        //如果队列为空，线程就退出
                        if(this->tasks_.empty())
                            return;
                        //取出头部任务给task,然后pop
                        task = std::move(this->tasks_.front());
                        this->tasks_.pop();       
                    }
                        this->thread_num_--;
                        task();
                        this->thread_num_++;
                }
            });
        }
   }
    //在析构时调用，等待所有线程执行结束
   void stop(){
    stop_.store(true);
    cv_lock_.notify_all();
    for(auto& td : pool_){
        if(td.joinable()){
            std::cout<<"join thread "<<td.get_id()<<std::endl;
            td.join();
        }
    }
   }

private:
    std::atomic_int             thread_num_;
    std::queue<Task>            tasks_;
    std::vector<std::thread>    pool_;
    std::atomic_bool            stop_;
    std::mutex                  cv_mt_;
    std::condition_variable     cv_lock_;
};



