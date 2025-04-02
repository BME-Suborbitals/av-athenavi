#include "mmc5983ma_task.h"
#include <cstdint>
#include "FreeRTOSConfig.h"
#include "communication/i2c_threadsafe.h"
#include "i2c.h"
#include "portmacro.h"
#include "projdefs.h"
#include "rtos_task.h"
#include "task_configuration.h"
#include "tasks/data_observer.h"

namespace tasks {
constexpr uint8_t MMC5983MA_DEVICE_ADDRESS = 0x30;

MMC5983MATask::MMC5983MATask()
    : rtos::Task("MMC5983MA", SENSOR_TASK_STACK, static_cast<UBaseType_t>(Priority::SENSOR)),
      i2c_device_(hi2c1, MMC5983MA_DEVICE_ADDRESS),
      sensor_(&i2c_device_) {}

void MMC5983MATask::Run() {
    TickType_t last_wake_time = xTaskGetTickCount();
    const TickType_t delay_ticks = pdMS_TO_TICKS(MAGNETO_FREQUENCY.count());

    sensor_.Initialize(sensor::MMC5983MA::BANDWIDTH_100_HZ);

    sensor::MMC5983MA::Data sensor_data{};
    while (true) {
        if (sensor_.Read(sensor_data)) {
            data_provider_.NotifyListeners(sensor_data);
        }
        xTaskDelayUntil(&last_wake_time, delay_ticks);
    }
}

void MMC5983MATask::RegisterListener(DataObserver<sensor::MMC5983MA::Data>& observer) {
    data_provider_.Subscribe(&observer);
}

}  // namespace tasks
