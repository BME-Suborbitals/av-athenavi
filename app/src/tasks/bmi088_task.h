#ifndef BMI088_TASK_H
#define BMI088_TASK_H

#include "bmi088.h"
#include "communication/i2c_threadsafe.h"
#include "data_observer.h"
#include "data_provider.h"
#include "i2c.h"
#include "rtos_task.h"

namespace tasks {
class BMI088Task : rtos::Task {
  private:
    DataProvider<sensor::BMI088::Data> data_provider_;
    communication::I2CDeviceThreadsafe i2c_acc_;
    communication::I2CDeviceThreadsafe i2c_gyr_;
    sensor::BMI088 sensor_;

  public:
    BMI088Task();
    void Run() override;
    void RegisterListener(DataObserver<sensor::BMI088::Data>& observer);
};
}  // namespace tasks

#endif  //BMI088_TASK_H