#ifndef LOG_TASK_H
#define LOG_TASK_H

#include <chrono>
#include "bme280.h"
#include "bmi088.h"
#include "data_observer.h"
#include "littlefs.h"
#include "mmc5983ma.h"
#include "monitored_task.h"
#include "ms561101ba03.h"
#include "portmacro.h"
#include "queue.h"

namespace tasks {
struct LogEntry {
    uint32_t timestamp;
    sensor::BMI088::Data imu_data;
    sensor::BME280::Data environment_data;
    sensor::MS561101BA03::Data barometric_data;
    sensor::MMC5983MA::Data magnetometer_data;
};

class LogTask : public tasks::MonitoredTask,
                public DataObserver<sensor::BMI088::Data>,
                public DataObserver<sensor::BME280::Data>,
                public DataObserver<sensor::MS561101BA03::Data>,
                public DataObserver<sensor::MMC5983MA::Data> {
  private:
    std::chrono::milliseconds log_frequency_;
    littlefs::LittleFS* file_system_;

    QueueHandle_t imu_queue_;
    QueueHandle_t env_queue_;
    QueueHandle_t baro_queue_;
    QueueHandle_t magneto_queue_;

    void LogData_();
    size_t UpdateBootCount_();

  public:
    LogTask(littlefs::LittleFS& file_system, std::chrono::milliseconds log_frequency, UBaseType_t priority, StackType_t stack_size);
    void Run() override;
    void OnDataReceived(const sensor::BMI088::Data& data);
    void OnDataReceived(const sensor::BME280::Data& data);
    void OnDataReceived(const sensor::MS561101BA03::Data& data);
    void OnDataReceived(const sensor::MMC5983MA::Data& data);
};
}  // namespace tasks

#endif  //LOG_TASK_H