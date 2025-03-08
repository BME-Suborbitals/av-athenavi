#include "log_task.h"
#include <array>
#include <chrono>
#include <cstddef>
#include "bme280.h"
#include "bmi088.h"
#include "lfs.h"
#include "littlefs.h"
#include "littlefs_file.h"
#include "mmc5983ma.h"
#include "monitored_task.h"
#include "ms561101ba03.h"
#include "portmacro.h"
#include "projdefs.h"
#include "rtos_task.h"
#include "semihosting_stream.h"
#include "stm32f4xx_hal.h"

namespace tasks {
constexpr int SYNC_INTERVAL = 10;  // Number of log entries before syncing to disk

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

LogTask::LogTask(littlefs::LittleFS& file_system, std::chrono::milliseconds log_frequency, UBaseType_t priority, StackType_t stack_size)
    : MonitoredTask("Log", stack_size, priority),
      file_system_(&file_system),
      log_frequency_(log_frequency),
      imu_queue_(xQueueCreate(1, sizeof(sensor::BMI088::Data))),
      env_queue_(xQueueCreate(1, sizeof(sensor::BME280::Data))),
      baro_queue_(xQueueCreate(1, sizeof(sensor::MS561101BA03::Data))),
      magneto_queue_(xQueueCreate(1, sizeof(sensor::MMC5983MA::Data))) {}

void LogTask::Run() {
    TickType_t last_wake_time = xTaskGetTickCount();
    const TickType_t delay_ticks = pdMS_TO_TICKS(log_frequency_.count());

    auto boot_count = UpdateBootCount_();
    mcu::semi << "Boot count: " << boot_count << "\n";

    std::array<char, littlefs::MAX_PATH_LENGTH> log_file_name{0};
    sprintf(log_file_name.data(), "log-%d", boot_count);
    littlefs::File log_file{log_file_name.data(), *file_system_};
    log_file.Open(LFS_O_RDWR | LFS_O_CREAT);

    int sync_counter = 0;
    LogEntry log_entry{};
    while (true) {
        xQueuePeek(imu_queue_, &log_entry.imu_data, 0);
        xQueuePeek(env_queue_, &log_entry.environment_data, 0);
        xQueuePeek(baro_queue_, &log_entry.barometric_data, 0);
        xQueuePeek(magneto_queue_, &log_entry.magnetometer_data, 0);
        log_entry.timestamp = HAL_GetTick();

        log_file.Write(&log_entry, sizeof(LogEntry));

        if (++sync_counter >= SYNC_INTERVAL) {
            log_file.Sync();
            sync_counter = 0;
        }

        Heartbeat();
        (void)xTaskDelayUntil(&last_wake_time, delay_ticks);
    }
}

void LogTask::OnDataReceived(const sensor::BMI088::Data& data) {
    xQueueOverwrite(imu_queue_, &data);
}

void LogTask::OnDataReceived(const sensor::BME280::Data& data) {
    xQueueOverwrite(env_queue_, &data);
}

void LogTask::OnDataReceived(const sensor::MS561101BA03::Data& data) {
    xQueueOverwrite(baro_queue_, &data);
}

void LogTask::OnDataReceived(const sensor::MMC5983MA::Data& data) {
    xQueueOverwrite(magneto_queue_, &data);
}
}  // namespace tasks
