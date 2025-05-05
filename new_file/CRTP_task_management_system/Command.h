#include<string>
#include<iostream>
#include<memory>
#pragma once

class CommandBase{
public:
    virtual void execute(const std::string& args) = 0;
};
// CRTP
template<typename Derived>
class Command:public CommandBase{
public:
    void execute(const std::string& args){
        (static_cast<Derived*>(this))->executeImpl(args);
    }
};


#include "TaskManager.h"
#include "Logger.h"

// 添加任务命令
class AddCommand: public Command<AddCommand>{
public:
    AddCommand(TaskManager& manager):taskManager(manager){}
    void executeImpl(const std::string& args){
        //简单的参数解析：描述,优先级,截止日期
        size_t pos1 = args.find(',');
        size_t pos2 = args.find(',', pos1 + 1);
        if (pos1 == std::string::npos || pos2 == std::string::npos) {
            std::cout << "参数格式错误。请使用: add <描述>,<优先级>,<截止日期>" << std::endl;
            return;
        }
        std::string description = args.substr(0, pos1);
        int priority = std::stoi(args.substr(pos1 + 1, pos2 - pos1 - 1));
        std::string dueDate = args.substr(pos2 + 1);
        taskManager.addTask(description, priority, dueDate);
        std::cout << "任务添加成功。" << std::endl;
    }
private:
    TaskManager& taskManager;
};

//删除
class DeleteCommand: public Command<DeleteCommand>{
public:
    DeleteCommand(TaskManager& manager):taskManager(manager){}
    void executeImpl(const std::string& args){
        try{
            size_t pos;
            int id = std::stoi(args, &pos);
            if(pos != args.length()){
                std::cout << "参数格式错误。请使用: delete <ID>" << std::endl;
                return;
            }
            taskManager.deleteTask(id);
            std::cout << "任务删除成功。" << std::endl;
        }catch(const std::invalid_argument& e){
            Logger::getInstance().log("参数格式错误");
            return;
        }catch(const std::out_of_range& e){
            Logger::getInstance().log("参数格式错误");
            return;
        }  
    }
private:
    TaskManager& taskManager;
};

//列出所有的任务
class ListCommand:public Command<ListCommand>{
public:
    ListCommand(TaskManager& manager):taskManager(manager){}
    void executeImpl(const std::string& args){
        try{
            int sortOption = 0;
            size_t pos;
            sortOption = std::stoi(args,&pos);
            if(pos != args.length()){
                Logger::getInstance().log("参数格式错误！");
                return;
            }
            std::cout<<"当前任务列表:"<<std::endl;
            taskManager.listTasks(sortOption);
        }catch(const std::invalid_argument& e){
            Logger::getInstance().log("参数格式错误");
            return;
        }catch(const std::out_of_range& e){
            Logger::getInstance().log("参数格式错误");
            return;
        }  
    }
private:
    TaskManager& taskManager;
};

// 更新任务命令
class UpdateCommand : public Command<UpdateCommand> {
    public:
        UpdateCommand(TaskManager& manager) : taskManager(manager) {}
        void executeImpl(const std::string& args) {
            // 参数格式: ID,描述,优先级,截止日期
            size_t pos1 = args.find(',');
            size_t pos2 = args.find(',', pos1 + 1);
            size_t pos3 = args.find(',', pos2 + 1);
            if (pos1 == std::string::npos || pos2 == std::string::npos || pos3 == std::string::npos) {
                std::cout << "参数格式错误。请使用: update <ID>,<描述>,<优先级>,<截止日期>" << std::endl;
                return;
            }
            int id = std::stoi(args.substr(0, pos1));
            std::string description = args.substr(pos1 + 1, pos2 - pos1 - 1);
            int priority = std::stoi(args.substr(pos2 + 1, pos3 - pos2 - 1));
            std::string dueDate = args.substr(pos3 + 1);
            taskManager.updateTask(id, description, priority, dueDate);
            std::cout << "任务更新成功。" << std::endl;
        }
    private:
        TaskManager& taskManager;
    };