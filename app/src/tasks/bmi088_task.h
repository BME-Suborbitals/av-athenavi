#ifndef BMI088_TASK_H
#define BMI088_TASK_H

#include "bmi088.h"
#include "communication/i2c_threadsafe.h"
#include "data_observer.h"
#include "data_provider.h"
#include "i2c.h"
#include "rtos_task.h"

namespace tasks {
/**
 * @brief Task managing the BMI088 inertial measurement unit (IMU)
 * 
 * This task periodically reads acceleration and gyroscope data from
 * the BMI088 sensor and distributes it to registered observers.
 */
class BMI088Task : rtos::Task {
  private:
    DataProvider<sensor::BMI088::Data> data_provider_;
    communication::I2CDeviceThreadsafe i2c_acc_;
    communication::I2CDeviceThreadsafe i2c_gyr_;
    sensor::BMI088 sensor_;

  public:
    /**
     * @brief Constructs a BMI088Task
     * 
     * Initializes the I2C communication and sensor driver for both 
     * accelerometer and gyroscope components
     */
    BMI088Task();

    /**
     * @brief Main task execution function
     * 
     * Periodically reads sensor data and distributes it to observers
     */
    void Run() override;

    /**
     * @brief Registers an observer to receive sensor data updates
     * @param observer The observer object that will receive IMU data
     */
    void RegisterListener(DataObserver<sensor::BMI088::Data>& observer);
};
}  // namespace tasks

#endif  //BMI088_TASK_H