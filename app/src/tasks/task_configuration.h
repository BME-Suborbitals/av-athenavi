#ifndef TASK_CONFIGURATION_H
#define TASK_CONFIGURATION_H

#include <chrono>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

namespace tasks {
constexpr std::chrono::milliseconds TASK_TIMEOUT(4500);
constexpr std::chrono::milliseconds LOG_FREQUENCY(100);

enum class Priority : UBaseType_t {
    WATCHDOG = configMAX_PRIORITIES - 1,
    LOG = 1,
    SENSOR = 2,
    BACKGROUND = tskIDLE_PRIORITY,
};

}  // namespace tasks

#endif  //TASK_CONFIGURATION_H