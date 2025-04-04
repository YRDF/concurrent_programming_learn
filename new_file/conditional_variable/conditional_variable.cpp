#include<iostream>
#include<mutex>
#include<thread>
#include <queue>
#include <condition_variable>


std::mutex mtx_num;
int num = 1;
std::condition_variable cvA;
std::condition_variable cvB;

void ResonableImplemention(){
    std::thread t1([](){
        for(;;){
            std::unique_lock<std::mutex> lock(mtx_num);
            cvA.wait(lock,[](){
                return num == 1;
            });
            num++;
            std::cout<<"thread A print 1...: "<<num<<std::endl;
            cvB.notify_one();
        }
    });

    std::thread t2([]() {
        for (;;) {

            std::unique_lock<std::mutex> lock(mtx_num);
            cvB.wait(lock, []() {
                return num == 2;
                });
            num--;
            std::cout << "thread B print 2.....: "<<num<<std::endl;
            cvA.notify_one();
        }
    });
    t1.join();
    t2.join();
}

//线程安全队列





int main(){
    //ResonableImplemention();
    return 0;
}