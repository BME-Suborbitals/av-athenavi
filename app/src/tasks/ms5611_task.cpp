#include "ms5611_task.h"
#include <cstdint>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "i2c.h"
#include "portmacro.h"
#include "projdefs.h"
#include "rtos_task.h"
#include "task.h"
#include "task_configuration.h"
#include "tasks/data_observer.h"

namespace tasks {
constexpr uint8_t MS5611_DEVICE_ADDRESS = 0x76;

MS5611Task::MS5611Task()
    : rtos::Task("MS5611", SENSOR_TASK_STACK, static_cast<UBaseType_t>(Priority::SENSOR)),
      i2c_device_(hi2c1, MS5611_DEVICE_ADDRESS),
      sensor_(&i2c_device_) {}

void MS5611Task::Run() {
    TickType_t last_wake_time = xTaskGetTickCount();
    const TickType_t delay_ticks = pdMS_TO_TICKS(BARO_FREQUENCY.count());

    sensor_.Initialize();

    sensor::MS561101BA03::Data sensor_data{};
    while (true) {
        if (sensor_.Read(sensor_data)) {
            data_provider_.NotifyListeners(sensor_data);
        }
        xTaskDelayUntil(&last_wake_time, delay_ticks);
    }
}

void MS5611Task::RegisterListener(DataObserver<sensor::MS561101BA03::Data>& observer) {
    data_provider_.Subscribe(&observer);
}

}  // namespace tasks
