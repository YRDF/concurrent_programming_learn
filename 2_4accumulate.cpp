#include<iostream>
#include<thread>
#include<vector>

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
    unsigned long const length = std::distance(first, last);
    if (!length)
        return init;    //⇽-- - ①
        unsigned long const min_per_thread = 25;
    unsigned long const max_threads =
        (length + min_per_thread - 1) / min_per_thread;    //⇽-- - ②
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