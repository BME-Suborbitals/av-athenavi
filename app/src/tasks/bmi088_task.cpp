#include "bmi088_task.h"
#include <cstdint>
#include "FreeRTOSConfig.h"
#include "communication/i2c_threadsafe.h"
#include "i2c.h"
#include "projdefs.h"
#include "rtos_task.h"
#include "tasks/data_observer.h"

namespace tasks {
constexpr uint8_t BMI088_ACCELEROMETER_ADDRESS = 0x19;
constexpr uint8_t BMI088_GYROSCOPE_ADDRESS = 0x69;

BMI088Task::BMI088Task()
    : rtos::Task("BMI088", configMINIMAL_STACK_SIZE, tskIDLE_PRIORITY),
      i2c_acc_(hi2c1, BMI088_ACCELEROMETER_ADDRESS),
      i2c_gyr_(hi2c1, BMI088_GYROSCOPE_ADDRESS),
      sensor_(&i2c_acc_, &i2c_gyr_) {}

void BMI088Task::Run() {
    volatile auto init_res = sensor_.Initialize(sensor::BMI088::ACC_RANGE_12_G, sensor::BMI088::SAMPLE_NORMAL, sensor::BMI088::ACC_ODR_100_HZ, sensor::BMI088::GYRO_RANGE_2000_DPS, sensor::BMI088::GYRO_ODR_1000_FBW_116);

    sensor::BMI088::Data sensor_data{};
    while (true) {
        if (sensor_.Read(sensor_data)) {
            data_provider_.NotifyListeners(sensor_data);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void BMI088Task::RegisterListener(DataObserver<sensor::BMI088::Data>& observer) {
    data_provider_.Subscribe(&observer);
}
}  // namespace tasks
