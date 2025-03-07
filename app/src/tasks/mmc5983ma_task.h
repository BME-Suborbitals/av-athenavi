#ifndef MMC5983MA_TASK_H
#define MMC5983MA_TASK_H

#include "communication/i2c_threadsafe.h"
#include "data_observer.h"
#include "data_provider.h"
#include "mmc5983ma.h"
#include "rtos_task.h"

namespace tasks {
class MMC5983MATask : public rtos::Task {
  private:
    DataProvider<sensor::MMC5983MA::Data> data_provider_;
    communication::I2CDeviceThreadsafe i2c_device_;
    sensor::MMC5983MA sensor_;

  public:
    MMC5983MATask();
    void Run() override;
    void RegisterListener(DataObserver<sensor::MMC5983MA::Data>& observer);
};
}  // namespace tasks

#endif  //MMC5983MA_TASK_H