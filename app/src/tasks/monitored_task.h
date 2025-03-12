#ifndef MONITORED_TASK_H
#define MONITORED_TASK_H

#include "FreeRTOS.h"
#include "portmacro.h"
#include "rtos_task.h"
#include "semphr.h"

namespace tasks {
/**
 * @brief Base class for tasks that need to be monitored by the watchdog
 * 
 * MonitoredTask extends rtos::Task with functionality that allows the
 * watchdog to monitor the task's health and detect if it becomes unresponsive.
 * Tasks that are critical for system operation should inherit from this class.
 */
class MonitoredTask : public rtos::Task {
  private:
    bool is_alive_{true};
    SemaphoreHandle_t is_ready_;

  protected:
    /**
     * @brief Signals that the task is still alive
     * 
     * This must be called periodically to ensure the watchdog doesn't
     * reset the system. It should be called in the task's main loop.
     */
    void Heartbeat();

    /**
     * @brief Marks the task as not ready
     * 
     * Can be used to allow tasks more time to initialize than the normal
     * watchdog period.
     */
    void NotReady();

    /**
     * @brief Marks the task as ready and operational
     * 
     * Called when the task has completed initialization and is 
     * ready to perform its functions
     */
    void Ready();

  public:
    /**
     * @brief Constructs a MonitoredTask
     * 
     * @param name The name of the task for debugging purposes
     * @param stack_size Size of the task's stack in words
     * @param priority The task's priority in the FreeRTOS scheduler
     */
    MonitoredTask(const char* name, StackType_t stack_size, UBaseType_t priority);

    /**
     * @brief Checks if the task is currently operational
     * 
     * Used by the watchdog to check if the task is running as expected
     * 
     * @return true if `Heartbeat` has been called since it has last expired
     */
    [[nodiscard]] bool IsAlive() const;

    /**
     * @brief Sets the task's alive status to false
     * 
     * Heartbeat needs to be called again before the next watchdog check
     */
    void ExpireHeartbeat();

    /**
     * @brief Waits until the task signals it's ready
     * 
     * @param timeout Maximum time to wait in ticks, default is indefinitely
     * @return true if the task became ready within the timeout, false otherwise
     */
    [[nodiscard]] bool WaitReady(uint32_t timeout = portMAX_DELAY);
};
}  // namespace tasks

#endif  //MONITORED_TASK_H