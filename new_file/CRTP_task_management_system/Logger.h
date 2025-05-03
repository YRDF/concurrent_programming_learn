#include<iostream>
#include<string>
#include<mutex>
#include<fstream>

class Logger{
public:
    //获取单例
    static Logger& getInstance();
    //禁止拷贝构造和拷贝赋值
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    //记录日志
    void log(const std::string& message);

    ~Logger();
private:
    Logger(); // 私有构造函数
    //向文件输出内容
    std::ofstream logFile;
    std::mutex mutex;
};