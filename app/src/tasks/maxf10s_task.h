#ifndef MMC5983MA_TASK_H
#define MMC5983MA_TASK_H

#include "communication/i2c_threadsafe.h"
#include "data_observer.h"
#include "data_provider.h"
#include "maxf10s.h"
#include "rtos_task.h"

namespace tasks {
/**
 * @brief Task managing the MMC5983MA magnetometer sensor
 * 
 * This task periodically reads magnetic field data from the MMC5983MA sensor
 * and distributes it to registered observers.
 */
class MAXF10STask : public rtos::Task {
  private:
    DataProvider<gnss::MAXF10S::Data> data_provider_;
    gnss::MAXF10S gnss_;

  public:
    /**
     * @brief Constructs an MAXF10STask
     */
    MAXF10STask();

    /**
     * @brief Main task execution function
     */
    void Run() override;

    /**
     * @brief Registers an observer to receive magnetometer data updates
     * @param observer The observer object that will receive magnetometer data
     */
    void RegisterListener(DataObserver<gnss::MAXF10S::Data>& observer);
};
}  // namespace tasks

#endif  //MMC5983MA_TASK_H