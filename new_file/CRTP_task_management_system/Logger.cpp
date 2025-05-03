#include"Logger.h"

Logger& Logger::getInstance(){
    static Logger logger;
    return logger;
}

//RAII思想：构造既初始化。构造打开文件，析构关闭文件。
Logger::Logger(){
    logFile.open("log.txt",std::ios::app);
    if(!logFile.is_open()){
        throw std::runtime_error("Failed to open log file!");
    }
}

Logger::~Logger(){
    if(logFile.is_open()){
        logFile.close();
    }
}

void Logger::log(const std::string& message){
    std::lock_guard<std::mutex> lock(mutex);
    if(logFile.is_open()){
        // 获取当前时间
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        char buffer[100];
        std::strftime(buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",std::localtime(&now_time));
        logFile << std::string(buffer) << ": " << message << std::endl;
    }
}