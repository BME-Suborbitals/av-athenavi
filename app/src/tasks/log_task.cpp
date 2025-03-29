#include "log_task.h"
#include <array>
#include <chrono>
#include <cstddef>
#include "bme280.h"
#include "bmi088.h"
#include "lfs.h"
#include "littlefs.h"
#include "littlefs_file.h"
#include "main.h"
#include "mmc5983ma.h"
#include "monitored_task.h"
#include "ms561101ba03.h"
#include "portmacro.h"
#include "projdefs.h"
#include "rtos_task.h"
#include "semihosting_stream.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "task_configuration.h"

namespace tasks {
constexpr int SYNC_INTERVAL = 10;  // Number of log entries before syncing to disk
constexpr size_t BULK_SIZE = 2048 / sizeof(LogEntry);

size_t LogTask::UpdateBootCount_() {
    littlefs::File boot_count_file("boot.cnt", *file_system_);
    boot_count_file.Open(LFS_O_RDWR | LFS_O_CREAT);
    size_t boot_count{0};
    auto read_result = boot_count_file.Read(&boot_count, sizeof(boot_count));
    if (read_result.has_value() && read_result.value() > 0) {
        boot_count++;
    }
    else {
        boot_count = 1;
    }
    boot_count_file.Rewind();
    boot_count_file.Write(&boot_count, sizeof(boot_count));
    boot_count_file.Close();

    return boot_count;
}

LogTask::LogTask(littlefs::LittleFS& file_system, std::chrono::milliseconds log_frequency, StackType_t stack_size)
    : MonitoredTask("Log", stack_size, static_cast<UBaseType_t>(Priority::LOG)),
      file_system_(&file_system),
      log_frequency_(log_frequency),
      giga_buffer_(xQueueCreate(100, sizeof(LogEntry))),
      busy_mutex(xSemaphoreCreateMutex()) {
    NotReady();
}

void LogTask::Run() {
    const TickType_t delay_ticks = pdMS_TO_TICKS(log_frequency_.count());

    auto boot_count = UpdateBootCount_();
    mcu::semi << "Boot count: " << boot_count << "\n";

    std::array<char, littlefs::MAX_PATH_LENGTH> log_file_name{0};
    sprintf(log_file_name.data(), "log-%04d", boot_count);
    littlefs::File log_file{log_file_name.data(), *file_system_};
    log_file.Open(LFS_O_RDWR | LFS_O_CREAT);

    Ready();

    int sync_counter = 0;
    LogEntry output_buffer[BULK_SIZE];

    while (true) {

        size_t items_collected = 0;

        // Try to fill the buffer
        while (items_collected < BULK_SIZE) {
            if (xQueueReceive(giga_buffer_, &output_buffer[items_collected], 0) == pdTRUE) {
                items_collected++;
            }
            else {
                break;  // No more data available
            }
        }

        if (xSemaphoreTake(busy_mutex, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        if (items_collected > 0) {
            log_file.Write(output_buffer, items_collected * sizeof(LogEntry));
            if (++sync_counter >= SYNC_INTERVAL) {
                log_file.Sync();
                sync_counter = 0;
                HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
            }
        }

        Heartbeat();
        xSemaphoreGive(busy_mutex);
        if (items_collected == 0) {
            vTaskDelay(delay_ticks);
        }
    }
}

void LogTask::Suspend() {
    if (xSemaphoreTake(busy_mutex, portMAX_DELAY) == pdTRUE) {
        rtos::Task::Suspend();
        xSemaphoreGive(busy_mutex);
    }
}

void LogTask::OnDataReceived(const sensor::BMI088::Data& data) {
    LogEntry entry{
        .timestamp = HAL_GetTick(),
        .data = data
    };
    xQueueSend(giga_buffer_, &entry, portMAX_DELAY);
}

void LogTask::OnDataReceived(const sensor::BME280::Data& data) {
    LogEntry entry{
        .timestamp = HAL_GetTick(),
        .data = data
    };
    xQueueSend(giga_buffer_, &entry, portMAX_DELAY);
}

void LogTask::OnDataReceived(const sensor::MS561101BA03::Data& data) {
    LogEntry entry{
        .timestamp = HAL_GetTick(),
        .data = data
    };
    xQueueSend(giga_buffer_, &entry, portMAX_DELAY);
}

void LogTask::OnDataReceived(const sensor::MMC5983MA::Data& data) {
    LogEntry entry{
        .timestamp = HAL_GetTick(),
        .data = data
    };
    xQueueSend(giga_buffer_, &entry, portMAX_DELAY);
}
}  // namespace tasks
