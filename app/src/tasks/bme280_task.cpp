#include "bme280_task.h"
#include <cstdint>
#include "FreeRTOSConfig.h"
#include "bme280.h"
#include "communication/i2c_threadsafe.h"
#include "i2c.h"
#include "projdefs.h"
#include "rtos_task.h"
#include "task_configuration.h"
#include "tasks/data_observer.h"

namespace tasks {
constexpr uint8_t BME280_DEVICE_ADDRESS = 0x77;

BME280Task::BME280Task()
    : rtos::Task("BME280", SENSOR_TASK_STACK, static_cast<UBaseType_t>(Priority::SENSOR)),
      i2c_device_(hi2c1, BME280_DEVICE_ADDRESS),
      sensor_(&i2c_device_) {}

void BME280Task::Run() {
    TickType_t last_wake_time = xTaskGetTickCount();
    const TickType_t delay_ticks = pdMS_TO_TICKS(ENV_FREQUENCY.count());

    sensor_.Initialize(sensor::BME280::Oversampling::SAMPLE_STANDARD, sensor::BME280::Oversampling::SAMPLE_STANDARD, sensor::BME280::Oversampling::SAMPLE_STANDARD, sensor::BME280::PowerMode::MODE_NORMAL, sensor::BME280::StandbyTime::STANDBY_62_5_MS, sensor::BME280::Filter::FILTER_2);

    sensor::BME280::Data sensor_data{};
    while (true) {
        if (sensor_.Read(sensor_data)) {
            data_provider_.NotifyListeners(sensor_data);
        }
        xTaskDelayUntil(&last_wake_time, delay_ticks);
    }
}

void BME280Task::RegisterListener(DataObserver<sensor::BME280::Data>& observer) {
    data_provider_.Subscribe(&observer);
}
}  // namespace tasks
