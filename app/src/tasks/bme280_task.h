#ifndef BME280_TASK_H
#define BME280_TASK_H

#include "bme280.h"
#include "communication/i2c_threadsafe.h"
#include "data_provider.h"
#include "rtos_task.h"
#include "tasks/data_observer.h"

namespace tasks {
class BME280Task : public rtos::Task {
  private:
    DataProvider<sensor::BME280::Data> data_provider_;
    communication::I2CDeviceThreadsafe i2c_device_;
    sensor::BME280 sensor_;

  public:
    BME280Task();
    void Run() override;
    void RegisterListener(DataObserver<sensor::BME280::Data>& observer);
};
}  // namespace tasks

#endif  //BME280_TASK_H