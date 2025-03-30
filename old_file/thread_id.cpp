#include<iostream>
#include<thread>

std::thread::id master_thread;
void some_core_part_of_algorithm()
{
 if(std::this_thread::get_id()==master_thread)
 {
    std::cout<<"master_thread work"<<std::endl;
 }
 std::cout<<"commom_ work "<<std::endl;
 std::cout<<std::this_thread::get_id();
}

int main()
{
    some_core_part_of_algorithm();
    return 0;
}