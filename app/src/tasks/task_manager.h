#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "tasks/bme280_task.h"
#include "tasks/bmi088_task.h"
#include "tasks/cli_task.h"
#include "tasks/log_task.h"
#include "tasks/mmc5983ma_task.h"
#include "tasks/ms5611_task.h"
#include "tasks/pitot_task.h"
#include "tasks/watchdog_task.h"

namespace tasks {
class TaskManager {
  private:
    TaskManager() = default;
    static TaskManager instance;
    tasks::BME280Task* bme280_task_{nullptr};
    tasks::BMI088Task* bmi088_task_{nullptr};
    tasks::MMC5983MATask* mmc5983ma_task_{nullptr};
    tasks::MS5611Task* ms5611_task_{nullptr};
    tasks::PitotTask* pitot_task_{nullptr};
    tasks::LogTask* log_task_{nullptr};
    tasks::WatchdogTask* watchdog_task_{nullptr};
    tasks::CLITask* cli_task_{nullptr};

  public:
    static TaskManager& GetInstance();
    void Start(littlefs::LittleFS& file_system);
    void EnterMaintenanceMode();
};
}  // namespace tasks

#endif  //TASK_MANAGER_H