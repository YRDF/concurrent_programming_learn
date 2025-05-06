#include<iostream>
#include"Logger.h"
#include"TaskManager.h"
#include"Command.h"
#include <unordered_map>
#include<functional>
#include<variant>
#include <windows.h>

int main(){
    // 设置控制台输出编码为 UTF-8
    SetConsoleOutputCP(65001);
    // std::cout<<"hello world!"<<std::endl;
    // Logger::getInstance().log("hello world!");
    // TaskManager tm1;
    // AddCommand addc(tm1);
    // addc.excute("study,1,2025-05-05");
    TaskManager taskManager;

    //方式1：虚基类
    // std::unordered_map<std::string,std::unique_ptr<CommandBase>> commands;
    // commands["add"] = std::make_unique<AddCommand>(taskManager);
    // commands["delete"] = std::make_unique<DeleteCommand>(taskManager);
    // commands["list"] = std::make_unique<ListCommand>(taskManager);
    // commands["update"] = std::make_unique<UpdateCommand>(taskManager);

    // //方式2:类型擦除
    // //可调用对象，返回值是void，参数是const std::string&
    // std::unordered_map<std::string,std::function<void(const std::string&)>> commands;

    // // 创建命令对象
    // auto add_command = std::make_shared<AddCommand>(taskManager);
    // auto delete_command = std::make_shared<DeleteCommand>(taskManager);
    // auto list_command = std::make_shared<ListCommand>(taskManager);
    // auto update_command = std::make_shared<UpdateCommand>(taskManager);

    // //放入map
    // commands["add"] = [add_command](const std::string& args)
    // { add_command->execute(args); };
    // commands["delete"] = [delete_command](const std::string& args)
    // { delete_command->execute(args); };
    // commands["list"] = [list_command](const std::string& args)
    // { list_command->execute(args); };
    // commands["update"] = [update_command](const std::string& args)
    // { update_command->execute(args); };

    //方式3 C++17使用std::any或std::variant (C++17)
    //使用std::variant
    //variant可以存不同类型
    using CommandVariant = std::variant<
            std::unique_ptr<AddCommand>,
            std::unique_ptr<DeleteCommand>,
            std::unique_ptr<ListCommand>,
            std::unique_ptr<UpdateCommand>
            >;
    std::unordered_map<std::string,CommandVariant> commands;
    commands["add"] = std::make_unique<AddCommand>(taskManager);
    commands["delete"] = std::make_unique<DeleteCommand>(taskManager);
    commands["list"] = std::make_unique<ListCommand>(taskManager);
    commands["update"] = std::make_unique<UpdateCommand>(taskManager);

    // // 调用
    // //std::visit([&args](auto&& cmd) { cmd.execute(args); }, commands["add"]);
    // // 使用std::any
    // std::unordered_map<std::string, std::any> commands;
    // commands["add"] = AddCommand(taskManager);
    // commands["delete"] = DeleteCommand(taskManager);

    // // 调用（需要知道具体类型）
    // std::any_cast<AddCommand&>(commands["add"]).execute("任务描述,1,2023-12-31");



    std::cout << "欢迎使用任务管理系统！" << std::endl;
    std::cout << "可用命令: add, delete, list, update, exit" << std::endl;

    std::string input;
    while (true) {
        std::cout << "\n> ";
        std::getline(std::cin, input);
        if (input.empty()) continue;

        // 分离命令和参数
        size_t spacePos = input.find(' ');
        std::string cmd = input.substr(0, spacePos);
        std::string args;
        //string::npos字符串结尾，如果不是结尾
        if (spacePos != std::string::npos) {
            args = input.substr(spacePos + 1);
        }

        if (cmd == "exit") {
            std::cout << "退出程序。" << std::endl;
            break;
        }
        //在commands中查询指令
        auto it = commands.find(cmd);
        if (it != commands.end()) {
            //it->second(args);
            std::visit([&args](auto&& cmd) { cmd->execute(args); }, it->second);
        } else {
            std::cout << "未知命令：" << cmd << std::endl;
        }
    }

    return 0;
}