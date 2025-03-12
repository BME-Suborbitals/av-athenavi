#ifndef BME280_TASK_H
#define BME280_TASK_H

#include "bme280.h"
#include "communication/i2c_threadsafe.h"
#include "data_provider.h"
#include "rtos_task.h"
#include "tasks/data_observer.h"

namespace tasks {
/**
 * @brief Task managing the BME280 environmental sensor
 * 
 * This task periodically reads temperature, humidity, and pressure data
 * from the BME280 sensor and distributes it to registered observers.
 */
class BME280Task : public rtos::Task {
  private:
    DataProvider<sensor::BME280::Data> data_provider_;
    communication::I2CDeviceThreadsafe i2c_device_;
    sensor::BME280 sensor_;

  public:
    /**
     * @brief Constructs a BME280Task
     * 
     * Initializes the I2C communication and sensor driver
     */
    BME280Task();

    /**
     * @brief Main task execution function
     * 
     * Periodically reads sensor data and distributes it to observers
     */
    void Run() override;

    /**
     * @brief Registers an observer to receive sensor data updates
     * @param observer The observer object that will receive sensor data
     */
    void RegisterListener(DataObserver<sensor::BME280::Data>& observer);
};
}  // namespace tasks

#endif  //BME280_TASK_H