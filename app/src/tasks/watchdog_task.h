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
/**
 * @brief Software watchdog task responsible for making sure all critical tasks
 * are running correctly, and resetting the hardware watchdog if everything is
 * nominal, triggering a system reset otherwise.
 */
class WatchdogTask : public rtos::Task {
  private:
    std::chrono::milliseconds task_timeout_;
    std::vector<MonitoredTask*> monitored_tasks_{};

  protected:
    /**
     * @brief Main execution function for the watchdog task
     * 
     * Continually checks the status of monitored tasks and resets
     * the hardware watchdog if all tasks are healthy
     */
    void Run() override;

  public:
    /**
     * @brief Constructs a WatchdogTask instance
     * @param task_timeout Maximum time a task can go without checking in
     * @param stack_size Size of the RTOS task stack in words
     * 
     * Creates a watchdog task that monitors other tasks for liveness
     */
    WatchdogTask(std::chrono::milliseconds task_timeout, StackType_t stack_size = configMINIMAL_STACK_SIZE);

    /**
     * @brief Adds a task to be monitored
     * @param task Pointer to the task to monitor
     * 
     * The watchdog will keep track of this task's health status and
     * include it in the overall system health determination
     */
    void RegisterTask(MonitoredTask* task);
};
}  // namespace tasks

#endif  //WATCHDOG_TASK_H