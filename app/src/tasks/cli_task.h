#ifndef CLI_TASK_H
#define CLI_TASK_H

#include <string>
#include <unordered_map>
#include "littlefs.h"
#include "rtos_task.h"

namespace tasks {
class CLITask : public rtos::Task {
  private:
    using CommandHandler = std::function<void(std::vector<std::string>&)>;

    struct CommandInfo {
        CommandHandler handler;
        std::string help_text{};
    };

    littlefs::LittleFS file_system_;
    std::unordered_map<std::string, CommandInfo> commands_{};
    static void Print_(const char* string);

    void GetCommand_();
    static std::vector<std::string> ParseCommand_(const std::string& input);
    void ProcessCommand_(std::vector<std::string>& args);

    void ShowHelp_();
    void ListFiles_();
    void DumpLog_(std::vector<std::string>& args);
    void FormatFileSystem_();
    void ShowFlashCapacity_();

  public:
    CLITask(littlefs::LittleFS& file_system);
    void Run() override;
    void RegisterCommand(const std::string& command, CommandHandler command_handler, const std::string& help = "");
};
}  // namespace tasks

#endif  //CLI_TASK_H