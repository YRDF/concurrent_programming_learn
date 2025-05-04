#include<iostream>
#include<string>
#include<sstream>
#include<iomanip>

class Task{
public:
    int id;                     //任务id 
    std::string description;    //描述
    int priority;               //任务优先级
    std::string dueDate;        //日期  格式: YYYY-MM-DD

    //把这些参数都转成字符串
    std::string toString() const{
        std::ostringstream oss;
        oss << "ID: " << id 
            << ", 描述: " << description 
            << ", 优先级: " << priority 
            << ", 截止日期: " << dueDate;
        return oss.str();
    }
};