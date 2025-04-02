#ifndef TASK_CONFIGURATION_H
#define TASK_CONFIGURATION_H

#include <chrono>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

namespace tasks {
constexpr std::chrono::milliseconds TASK_TIMEOUT(4500);
constexpr std::chrono::milliseconds LOG_FREQUENCY(50);

constexpr std::chrono::milliseconds IMU_FREQUENCY(40);
constexpr std::chrono::milliseconds ENV_FREQUENCY(250);
constexpr std::chrono::milliseconds BARO_FREQUENCY(60);
constexpr std::chrono::milliseconds MAGNETO_FREQUENCY(60);
constexpr std::chrono::milliseconds PITOT_FREQUENCY(100);

constexpr StackType_t SENSOR_TASK_STACK = 500;
constexpr StackType_t CLI_TASK_STACK = configMINIMAL_STACK_SIZE + 1000;
constexpr StackType_t LOG_TASK_STACK = configMINIMAL_STACK_SIZE + 3000;

enum class Priority : uint8_t {
    WATCHDOG = configMAX_PRIORITIES - 1,
    LOG = tskIDLE_PRIORITY + 10,
    SENSOR = tskIDLE_PRIORITY + 20,
    CLI = tskIDLE_PRIORITY + 1,
    BACKGROUND = tskIDLE_PRIORITY,
};

}  // namespace tasks

#endif  //TASK_CONFIGURATION_H