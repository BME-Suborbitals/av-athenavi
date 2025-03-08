#ifndef MS5611_TASK_H
#define MS5611_TASK_H

#include "communication/i2c_threadsafe.h"
#include "data_observer.h"
#include "data_provider.h"
#include "ms561101ba03.h"
#include "rtos_task.h"

namespace tasks {
class MS5611Task : public rtos::Task {
  private:
    DataProvider<sensor::MS561101BA03::Data> data_provider_;
    communication::I2CDeviceThreadsafe i2c_device_;
    sensor::MS561101BA03 sensor_;

  public:
    MS5611Task();
    void Run() override;
    void RegisterListener(DataObserver<sensor::MS561101BA03::Data>& observer);
};
}  // namespace tasks

#endif  //MS5611_TASK_H