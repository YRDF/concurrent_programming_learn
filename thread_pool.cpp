#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <atomic>
#include <condition_variable>
#include <future>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool  {
public:
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    static ThreadPool& instance() {
        static ThreadPool ins;
        return ins;
    }

    using Task = std::packaged_task<void()>;

    ~ThreadPool() {
        stop();
    }

    template <class F, class... Args>
    auto commit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
        using RetType = decltype(f(args...));
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

    int idleThreadCount() {
        return thread_num_;
    }

private:
    ThreadPool(unsigned int num = 5)
        : stop_(false) {
            {
                if (num < 1)
                    thread_num_ = 1;
                else
                    thread_num_ = num;
            }
            start();
    }
    void start() {
        for (int i = 0; i < thread_num_; ++i) {
            pool_.emplace_back([this]() {
                while (!this->stop_.load()) {
                    Task task;
                    {
                        std::unique_lock<std::mutex> cv_mt(cv_mt_);
                        this->cv_lock_.wait(cv_mt, [this] {
                            return this->stop_.load() || !this->tasks_.empty();
                        });
                        if (this->tasks_.empty())
                            return;

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
    void stop() {
        stop_.store(true);
        cv_lock_.notify_all();
        for (auto& td : pool_) {
            if (td.joinable()) {
                std::cout << "join thread " << td.get_id() << std::endl;
                td.join();
            }
        }
    }

private:
    std::mutex               cv_mt_;
    std::condition_variable  cv_lock_;
    std::atomic_bool         stop_;
    std::atomic_int          thread_num_;
    std::queue<Task>         tasks_;
    std::vector<std::thread> pool_;
};

#endif  // !__THREAD_POOL_H__

int main()
{
    int m = 0;
    ThreadPool::instance().commit([](int& m){
        m = 1024;
        std::cout<<"inner set m is "<<m<<std::endl;
        std::cout<<"m addres is "<<&m<<std::endl;
    },std::ref(m));

    int m2 = 10;
    ThreadPool::instance().commit([](int& m2){
        m2 = 103;
        std::cout<<"inner set m2 is "<<m2<<std::endl;
        std::cout<<"m2 addres is "<<&m2<<std::endl;
    },std::ref(m2));

    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout<<"m is "<<m<<std::endl;
    std::cout<<"m addres is "<<&m<<std::endl;
    
    std::this_thread::sleep_for(std::chrono::seconds(3));

    return 0;
}