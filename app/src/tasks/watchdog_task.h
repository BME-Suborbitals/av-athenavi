#ifndef WATCHDOG_TASK_H
#define WATCHDOG_TASK_H

#include <chrono>
#include <vector>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "monitored_task.h"
#include "portmacro.h"
#include "rtos_task.h"

namespace tasks {
class WatchdogTask : public rtos::Task {
  private:
    std::chrono::milliseconds task_timeout_;
    std::vector<MonitoredTask*> monitored_tasks_{};

  protected:
    void Run() override;

  public:
    WatchdogTask(std::chrono::milliseconds task_timeout, UBaseType_t priority = configMAX_PRIORITIES - 1, StackType_t stack_size = configMINIMAL_STACK_SIZE);

    /**
     * @brief Adds a task to be monitored.
     * @param task The task to monitor
     */
    void RegisterTask(MonitoredTask* task);
};
}  // namespace tasks

#endif  //WATCHDOG_TASK_H