#ifndef MMC5983MA_TASK_H
#define MMC5983MA_TASK_H

#include "communication/i2c_threadsafe.h"
#include "data_observer.h"
#include "data_provider.h"
#include "mmc5983ma.h"
#include "rtos_task.h"

namespace tasks {
/**
 * @brief Task managing the MMC5983MA magnetometer sensor
 * 
 * This task periodically reads magnetic field data from the MMC5983MA sensor
 * and distributes it to registered observers.
 */
class MMC5983MATask : public rtos::Task {
  private:
    DataProvider<sensor::MMC5983MA::Data> data_provider_;
    communication::I2CDeviceThreadsafe i2c_device_;
    sensor::MMC5983MA sensor_;

  public:
    /**
     * @brief Constructs an MMC5983MATask
     * 
     * Initializes the I2C communication and sensor driver
     */
    MMC5983MATask();

    /**
     * @brief Main task execution function
     * 
     * Periodically reads magnetometer data and distributes it to observers
     */
    void Run() override;

    /**
     * @brief Registers an observer to receive magnetometer data updates
     * @param observer The observer object that will receive magnetometer data
     */
    void RegisterListener(DataObserver<sensor::MMC5983MA::Data>& observer);
};
}  // namespace tasks

#endif  //MMC5983MA_TASK_H