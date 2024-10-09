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
        unsigned long const hardware_threads =
        std::thread::hardware_concurrency();
    unsigned long const num_threads =
        std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);    //⇽-- - ③
        unsigned long const block_size = length / num_threads;    //⇽-- - ④
        std::vector<T> results(num_threads);
    std::vector<std::thread>  threads(num_threads - 1);   // ⇽-- - ⑤
        Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); ++i)
    {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);    //⇽-- - ⑥
            threads[i] = std::thread(//⇽-- - ⑦
                accumulate_block<Iterator, T>(),
                block_start, block_end, std::ref(results[i]));
        block_start = block_end;    //⇽-- - ⑧
    }
    accumulate_block<Iterator, T>()(
        block_start, last, results[num_threads - 1]);    //⇽-- - ⑨

        for (auto& entry : threads)
            entry.join();    //⇽-- - ⑩
            return std::accumulate(results.begin(), results.end(), init);    //⇽-- - ⑪
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