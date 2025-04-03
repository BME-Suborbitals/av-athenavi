#ifndef LOG_TASK_H
#define LOG_TASK_H

#include <chrono>
#include <variant>
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
/**
 * @brief Structure containing all sensor data to be logged in a single entry
 */
struct LogEntry {
    uint32_t timestamp;
    std::variant<
        sensor::BMI088::Data,
        sensor::BME280::Data,
        sensor::MS561101BA03::Data,
        sensor::MMC5983MA::Data>
        data;
};

/**
 * @brief Task responsible for logging sensor data to the flash
 */
class LogTask : public tasks::MonitoredTask,
                public DataObserver<sensor::BMI088::Data>,
                public DataObserver<sensor::BME280::Data>,
                public DataObserver<sensor::MS561101BA03::Data>,
                public DataObserver<sensor::MMC5983MA::Data> {
  private:
    std::chrono::milliseconds log_frequency_;
    littlefs::LittleFS* file_system_;

    QueueHandle_t giga_buffer_;

    SemaphoreHandle_t busy_mutex;

    /**
     * @brief Increments and returns the boot count stored in the file system
     * @return The updated boot count
     */
    size_t UpdateBootCount_();

  public:
    /**
     * @brief Constructs a LogTask instance
     * @param file_system Reference to the initialized LittleFS filesystem
     * @param log_frequency How often data should be logged
     * @param stack_size Size of the RTOS task stack in words
     */
    LogTask(littlefs::LittleFS& file_system, std::chrono::milliseconds log_frequency, StackType_t stack_size);

    /**
     * @brief Main task execution function
     * 
     * This function implements the main logging loop that runs continuously
     */
    void Run() override;

    void Suspend() override;

    /**
     * @brief Handler for receiving IMU data
     * @param data The IMU sensor data
     */
    void OnDataReceived(const sensor::BMI088::Data& data);

    /**
     * @brief Handler for receiving environmental data
     * @param data The environmental sensor data
     */
    void OnDataReceived(const sensor::BME280::Data& data);

    /**
     * @brief Handler for receiving barometric data
     * @param data The barometric pressure sensor data
     */
    void OnDataReceived(const sensor::MS561101BA03::Data& data);

    /**
     * @brief Handler for receiving magnetometer data
     * @param data The magnetometer sensor data
     */
    void OnDataReceived(const sensor::MMC5983MA::Data& data);
};
}  // namespace tasks

#endif  //LOG_TASK_H