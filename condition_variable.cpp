#include<iostream>
#include<thread>
#include<memory>
#include<mutex>
#include<condition_variable>

int num = 1;
std::mutex mtx_num;
std::condition_variable cvA;
std::condition_variable cvB;

void Resonablelmplemention(){
    std::thread t1([](){
        for(;;){
        std::unique_lock<std::mutex> lock(mtx_num);
        //使用条件变量
        cvA.wait(lock,[](){
            //如果num不等于1就return false线程就挂起，解锁。等待true
            return num == 1;
        });
        //还可以这样写
        /* while(num==1){
            cvA.wait(lock);
        } */

        num++;
        std::cout<<"thread A print 1..."<<std::endl;
        //notify通知线程b
        cvB.notify_one();
        }
    });

    std::thread t2([](){
        for(;;){
        std::unique_lock<std::mutex> lock (mtx_num);
        cvB.wait(lock,[](){
            return num == 2;
        });
        num--;
        std::cout<<"thread B print 2..."<<std::endl;
        cvA.notify_one();
        }
    });

    t1.join();
    t2.join();
}


int main()
{
    Resonablelmplemention();
    return 0;
}