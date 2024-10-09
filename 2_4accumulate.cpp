#include<iostream>
#include<thread>
#include<vector>
#include<numeric>

//借用C++标准库的std::thread::hardware_concurrency()函数，它的返回值是一个指标，
//表示程序在各次运行中可真正并发的线程数量. 
//我们可以模拟实现一个并行计算的功能，计算容器内所有元素的和
template<typename iterator,typename T>
struct accumulate_block
{
    void operator()(iterator first,iterator last, T&result)
    {
        result = std::accumulate(first,last,result);
    }
};

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
    unsigned long const length = std::distance(first, last);
    if (!length)
        return init;    //如果输入为空 就返回
        unsigned long const min_per_thread = 25;
    unsigned long const max_threads =
        (length + min_per_thread - 1) / min_per_thread;    //如果范围内的元素多于一个时，需要用范围内元素的总数量除以线程(块)中最小任务数，从而确定启动线程的最大数量
        //算出cpu核数
        unsigned long const hardware_threads =
        std::thread::hardware_concurrency();
    //较小的值为启动线程的数量
    unsigned long const num_threads =
        std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
        unsigned long const block_size = length / num_threads;
           //创建一个 std::vector<T> 容器存放中间结果，并为线程创建一个std::vector<std::thread>容器 
        std::vector<T> results(num_threads);
    std::vector<std::thread>  threads(num_threads - 1);
        Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); ++i)
    {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);    //使用循环来启动线程：block_end迭代器指向当前块的末尾
            threads[i] = std::thread(//并启动一个新线程为当前块累加结果
                accumulate_block<Iterator, T>(),
                block_start, block_end, std::ref(results[i]));
        block_start = block_end;    //当迭代器指向当前块的末尾时，启动下一个块
    }
    accumulate_block<Iterator, T>()(
        block_start, last, results[num_threads - 1]);    //此线程会处理最终块的结果

        for (auto& entry : threads)
            entry.join();    //累加最终块的结果后，可等待 std::for_each
            return std::accumulate(results.begin(), results.end(), init);    //使用 std::accumulate 将所有结果进行累加
}

void use_parallel_acc() {
    std::vector <int> vec(10000);
    for (int i = 0; i < 10000; i++) {
        vec.push_back(i);
    }
    int sum = 0;
    sum = parallel_accumulate<std::vector<int>::iterator, int>(vec.begin(), 
        vec.end(), sum);

    std::cout << "sum is " << sum << std::endl;
}

int main()
{
    use_parallel_acc();
    return 0;
}