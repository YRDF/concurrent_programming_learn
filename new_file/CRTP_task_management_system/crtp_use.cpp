#include<iostream>
#include"Logger.h"
#include"TaskManager.h"
#include"Command.h"

int main(){
    // std::cout<<"hello world!"<<std::endl;
    // Logger::getInstance().log("hello world!");
    TaskManager tm1;
    AddCommand addc(tm1);
    addc.excute("study,1,2025-05-05");

    return 0;
}