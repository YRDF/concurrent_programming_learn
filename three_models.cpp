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