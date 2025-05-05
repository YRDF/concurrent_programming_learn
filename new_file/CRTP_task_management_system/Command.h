#include<string>
#include<iostream>
#include<memory>
#pragma once
// CRTP
template<typename Derived>
class Command{
public:
    void excute(const std::string& args){
        (static_cast<Derived*>(this))->excuteImpl(args);
    }
};


#include "TaskManager.h"
#include "Logger.h"

// 添加任务命令
class AddCommand: public Command<AddCommand>{
public:
    AddCommand(TaskManager& manager):TaskManager(manager){}
    void excuteImpl(const std::string& args){
        //锟斤拷锟斤拷锟斤拷锟斤拷
        size_t pos1 = args.find(',');
        size_t pos2 = args.find(',', pos1 + 1);
        if (pos1 == std::string::npos || pos2 == std::string::npos) {
            std::cout << "参数格式错误。请使用: add <描述>,<优先级>,<截止日期>" << std::endl;
            return;
        }
        std::string description = args.substr(0, pos1);
        int priority = std::stoi(args.substr(pos1 + 1, pos2 - pos1 - 1));
        std::string dueDate = args.substr(pos2 + 1);
        TaskManager.addTask(description, priority, dueDate);
        std::cout << "任务添加成功。" << std::endl;
    }
private:
    TaskManager& TaskManager;
};
