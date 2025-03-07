#ifndef DUMMYBME280_TASK_H
#define DUMMYBME280_TASK_H

#include "bme280.h"
#include "communication/i2c_threadsafe.h"
#include "data_provider.h"
#include "rtos_task.h"
#include "tasks/data_provider.h"

namespace tasks {
class DummyBME280 : public rtos::Task {
  private:
    DataProvider<sensor::BME280::Data> data_provider_;
    sensor::BME280::Data MakeData_();

  public:
    DummyBME280();
    void Run() override;
    void RegisterListeners(DataObserver<sensor::BME280::Data>& dataobserver);
};
}  // namespace tasks

#endif