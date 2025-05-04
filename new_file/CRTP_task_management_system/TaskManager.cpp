#include"TaskManager.h"
#include"Logger.h"
#include<iostream>

TaskManager::TaskManager():nextId(1){
    loadTasks();
}

void TaskManager::loadTasks(){
    std::ifstream inFile("log.txt");
    if(!inFile.is_open()){
        Logger::getInstance().log("Failed to open tasks file.");
        return;
    }

    std::string line;
    while(std::getline(inFile,line)){
        std::istringstream iss(line);
        Task task;
        char delimiter;
        iss>>line>>delimiter;
        std::getline(iss,task.description,',');
        iss >> task.priority >> delimiter;
        iss >> task.dueDate;
        tasks.push_back(task);
        if (task.id >= nextId) {
            nextId = task.id + 1;
        }
    }
    inFile.close();
    Logger::getInstance().log("Tasks loaded successfully.");
}