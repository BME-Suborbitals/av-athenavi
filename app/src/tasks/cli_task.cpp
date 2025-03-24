#include "cli_task.h"
#include <array>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstring>
// #include <fmt/format.h>
#include <string>
#include <vector>
#include "FreeRTOSConfig.h"
#include "littlefs.h"
#include "littlefs_file.h"
#include "log_task.h"
#include "rtos_task.h"
#include "task.h"
#include "usbd_cdc_if.h"
#include "usbd_def.h"

void tasks::CLITask::Print_(const char* string) {
    while (CDC_Transmit_FS(reinterpret_cast<uint8_t*>(const_cast<char*>(string)), strlen(string)) != USBD_OK) {}
}

void tasks::CLITask::GetCommand_() {
    std::string input{reinterpret_cast<char*>(UserRxBufferFS)};
    memset(static_cast<uint8_t*>(UserRxBufferFS), 0, APP_RX_DATA_SIZE);

    Print_(input.c_str());

    // Skip leading whitespace
    size_t start = 0;
    while (start < input.size() && isspace(input[start])) {
        start++;
    }

    if (start >= input.size()) {
        // Empty command, nothing to do
        return;
    }

    // Parse the arguments
    std::vector<std::string> args;
    std::string current_arg = "";
    bool in_quotes = false;

    for (size_t i = start; i < input.size(); ++i) {
        char character = input[i];

        if (character == '"') {
            in_quotes = !in_quotes;
        }
        else if ((isspace(character) != 0) && !in_quotes) {
            if (!current_arg.empty()) {
                args.push_back(current_arg);
                current_arg.clear();
            }
        }
        else {
            current_arg += character;
        }
    }

    // Add the last argument if there is one
    if (!current_arg.empty()) {
        args.push_back(current_arg);
    }

    ProcessCommand_(args);
    Print_("> ");
}

void tasks::CLITask::ProcessCommand_(std::vector<std::string>& args) {
    if (args.size() < 1) {
        return;
    }

    auto command_it = commands_.find(args[0]);
    if (command_it != commands_.end()) {
        command_it->second.handler(args);
    }
    else {
        std::array<char, 128> error_msg;
        sprintf(error_msg.data(), "Unknown command: %s\r\nType 'help' for available commands\r\n", args[0].c_str());
        Print_(error_msg.data());
    }
}

void tasks::CLITask::ShowHelp_() {
    Print_("Available commands:\r\n");
    for (const auto& [cmd, info] : commands_) {
        char buff[128];
        sprintf(buff, "  %-10s - %s\r\n", cmd.c_str(), info.help_text.c_str());
        Print_(buff);
    }
}

void tasks::CLITask::ListFiles_() {
    // First pass: find the longest filename
    size_t max_name_length = 4;  // "Name" header length

    file_system_.ForEachFile([&max_name_length](const littlefs::FileInfo& file_info) {
        const size_t name_length = strlen(file_info.name);
        max_name_length = std::max(max_name_length, name_length);
    });

    // Add some extra padding and ensure minimum width
    max_name_length = std::max(max_name_length, static_cast<size_t>(10));
    max_name_length += 2;  // Add padding

    // Print header
    char header[128];
    sprintf(header, "%-*s %-9s %10s\r\n", static_cast<int>(max_name_length), "Name", "Type", "Size");
    Print_(header);

    // Print separator line
    char separator[128];
    memset(separator, '-', max_name_length);
    separator[max_name_length] = '\0';
    char separator_line[128];
    sprintf(separator_line, "%s ---------- ----------\r\n", separator);
    Print_(separator_line);

    // Print files with dynamic width
    file_system_.ForEachFile([this, max_name_length](const littlefs::FileInfo& file_info) {
        char buff[128];
        sprintf(buff, "%-*s %-9s %10d\r\n", static_cast<int>(max_name_length), file_info.name, file_info.is_directory ? "dir" : "file", file_info.size);
        Print_(buff);
    });
}

void tasks::CLITask::DumpLog_(std::vector<std::string>& args) {
    littlefs::File log_file{args[1].c_str(), file_system_};
    auto error = log_file.Open(LFS_O_RDONLY);
    if (!error.has_value()) {
        char error_msg[50];
        sprintf(error_msg, "Unable to open %s, error: %d\r\n", args[1].c_str(), error.error());
        Print_(error_msg);
        return;
    }
    tasks::LogEntry log_entry;
    char buff[150];
    std::expected<size_t, littlefs::LFSError> read_result = 1;
    while (read_result.has_value() && read_result.value() > 0) {
        read_result = log_file.Read(&log_entry, sizeof(log_entry));
        sprintf(buff, "%d;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f\n", log_entry.timestamp, log_entry.imu_data.time, log_entry.imu_data.temperature, log_entry.imu_data.acceleration_x, log_entry.imu_data.acceleration_y, log_entry.imu_data.acceleration_z, log_entry.imu_data.angular_velocity_x, log_entry.imu_data.angular_velocity_y, log_entry.imu_data.angular_velocity_z, log_entry.barometric_data.pressure, log_entry.barometric_data.temperature, log_entry.magnetometer_data.magnetic_field_x, log_entry.magnetometer_data.magnetic_field_y, log_entry.magnetometer_data.magnetic_field_z);
        CDC_Transmit_FS((uint8_t*)buff, strlen(buff));
    }
    log_file.Close();
}

void tasks::CLITask::FormatFileSystem_() {
    file_system_.Format();
    Print_("File system formatted.\nReseting...");
    vTaskDelay(pdMS_TO_TICKS(250));
    NVIC_SystemReset();
}

void tasks::CLITask::ShowFlashCapacity_() {
    auto result = file_system_.GetInfo();
    if (!result.has_value()) {
        Print_("Unable to read file system information.\r\n");
        return;
    }

    // Convert values to KB for better readability
    auto fs_info = result.value();
    float total_kb = (float)fs_info.total_size / 1024.0f;
    float used_kb = (float)fs_info.used / 1024.0f;
    float avail_kb = (float)fs_info.available / 1024.0f;

    // Print header with fixed column widths
    Print_("┌─────────────────┬─────────────────┬─────────────────┬──────────┐\r\n");
    Print_("│ Total (KB)      │ Used (KB)       │ Available (KB)  │ Used (%) │\r\n");
    Print_("├─────────────────┼─────────────────┼─────────────────┼──────────┤\r\n");

    // Print data row with proper alignment
    char buff[128];
    sprintf(buff, "│ %-15.2f │ %-15.2f │ %-15.2f │ %-7.2f%% │\r\n", total_kb, used_kb, avail_kb, fs_info.used_pct);
    Print_(buff);

    Print_("└─────────────────┴─────────────────┴─────────────────┴──────────┘\r\n");
}

tasks::CLITask::CLITask(littlefs::LittleFS& file_system)
    : rtos::Task("USB", configMINIMAL_STACK_SIZE + 1000, tskIDLE_PRIORITY),
      file_system_(file_system) {
    RegisterCommand("help", [this](std::vector<std::string>& args) { this->ShowHelp_(); }, "Show available commands");
    RegisterCommand("ls", [this](std::vector<std::string>& args) { this->ListFiles_(); }, "Lists all files in the root directory");
    RegisterCommand("dump", [this](std::vector<std::string>& args) { this->DumpLog_(args); }, "Dump a file's contents (usage: dump <filename>)");
    RegisterCommand("format", [this](std::vector<std::string>& args) { this->FormatFileSystem_(); }, "Format the file system and reboot");
    RegisterCommand("df", [this](std::vector<std::string>& args) { this->ShowFlashCapacity_(); }, "Display information on the flash usage");
}

extern USBD_HandleTypeDef hUsbDeviceFS;
extern bool send_motd;

void tasks::CLITask::Run() {
    while (true) {
        if (send_motd) {
            vTaskDelay(pdMS_TO_TICKS(50));
            Print_("\n============== [AtheNAVI®] ==============\n");
            Print_("Type 'help' for the list of available commands\n\n");
            Print_("> ");
            send_motd = false;
        }
        GetCommand_();
    }
}

void tasks::CLITask::RegisterCommand(const std::string& command, CommandHandler command_handler, const std::string& help) {
    commands_[command] = {
        .handler = command_handler,
        .help_text = help
    };
}
