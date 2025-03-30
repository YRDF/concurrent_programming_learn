#include <iostream>
#include <atomic>
#include <thread>
#include<cassert>
#include<vector>

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x_then_y() {
    x.store(true, std::memory_order_relaxed);  // 1
    y.store(true, std::memory_order_relaxed);  // 2
}

void read_y_then_x() {
    while (!y.load(std::memory_order_relaxed)) { // 3
        std::cout << "y load false" << std::endl;
    }

    if (x.load(std::memory_order_relaxed)) { //4
        ++z;
    }

}

void TestOrderRelaxed() {

    std::thread t1(write_x_then_y);
    std::thread t2(read_y_then_x);
    t1.join();
    t2.join();
    assert(z.load() != 0); // 5
}


void write_x_then_y() {
    x.store(true, std::memory_order_seq_cst);  // 1
    y.store(true, std::memory_order_seq_cst);  // 2
}

void read_y_then_x() {
    while (!y.load(std::memory_order_seq_cst)) { // 3
        std::cout << "y load false" << std::endl;
    }

    if (x.load(std::memory_order_seq_cst)) { //4
        ++z;
    }

}

void TestOrderSeqCst() {

    std::thread t1(write_x_then_y);
    std::thread t2(read_y_then_x);
    t1.join();
    t2.join();
    assert(z.load() != 0); // 5
}

//同步模型Acquire-Release
void TestReleaseAcquire() {
    std::atomic<bool> rx, ry;

    std::thread t1([&]() {
        rx.store(true, std::memory_order_relaxed); // 1
        ry.store(true, std::memory_order_release); // 2
        });


    std::thread t2([&]() {
        while (!ry.load(std::memory_order_acquire)); //3
        assert(rx.load(std::memory_order_relaxed)); //4
        });

    t1.join();
    t2.join();
}

void ReleaseSequence() {
    std::vector<int> data;
    std::atomic<int> flag{ 0 };

    std::thread t1([&]() {
        data.push_back(42);  //(1)
        flag.store(1, std::memory_order_release); //(2)
        });

    std::thread t2([&]() {
        int expected = 1;
        while (!flag.compare_exchange_strong(expected, 2, std::memory_order_relaxed)) // (3)
            expected = 1;
        });

    std::thread t3([&]() {
        while (flag.load(std::memory_order_acquire) < 2); // (4)
        assert(data.at(0) == 42); // (5)
        });

    t1.join();
    t2.join();
    t3.join();
}

int main()
{
    //TestOrderSeqCst();
    return 0;
}