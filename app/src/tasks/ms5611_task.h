#ifndef MS5611_TASK_H
#define MS5611_TASK_H

#include "communication/i2c_threadsafe.h"
#include "data_observer.h"
#include "data_provider.h"
#include "ms561101ba03.h"
#include "rtos_task.h"

namespace tasks {
/**
 * @brief Task managing the MS5611 barometric pressure sensor
 * 
 * This task periodically reads pressure and temperature data from
 * the MS5611 sensor and distributes it to registered observers.
 */
class MS5611Task : public rtos::Task {
  private:
    DataProvider<sensor::MS561101BA03::Data> data_provider_;
    communication::I2CDeviceThreadsafe i2c_device_;
    sensor::MS561101BA03 sensor_;

  public:
    /**
     * @brief Constructs an MS5611Task
     * 
     * Initializes the I2C communication and sensor driver
     */
    MS5611Task();

    /**
     * @brief Main task execution function
     * 
     * Periodically reads barometric data and distributes it to observers
     */
    void Run() override;

    /**
     * @brief Registers an observer to receive barometric data updates
     * @param observer The observer object that will receive pressure data
     */
    void RegisterListener(DataObserver<sensor::MS561101BA03::Data>& observer);
};
}  // namespace tasks

#endif  //MS5611_TASK_H