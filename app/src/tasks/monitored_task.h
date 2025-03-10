#ifndef MONITORED_TASK_H
#define MONITORED_TASK_H

#include "FreeRTOS.h"
#include "portmacro.h"
#include "rtos_task.h"
#include "semphr.h"

namespace tasks {
class MonitoredTask : public rtos::Task {
  private:
    bool is_alive_{true};
    SemaphoreHandle_t is_ready_;

  protected:
    /**
     * @brief Signals that the task is still alive. This must be called 
     * periodically to ensure the watchdog doesn't reset the system.
     */
    void Heartbeat();

    void NotReady();
    void Ready();

  public:
    MonitoredTask(const char* name, StackType_t stack_size, UBaseType_t priority);

    /**
     * @brief Used by the watchdog to check if the task is running as expected
     * @return true, if `Heartbeat` has been called since it has last expired.
     */
    [[nodiscard]] bool IsAlive() const;

    /**
     * @brief Sets `IsAlive` to fasle. `Heartbeat` needs to bee called again
     * before the next check.
     */
    void ExpireHeartbeat();

    [[nodiscard]] bool WaitReady(uint32_t timeout = portMAX_DELAY);
};
}  // namespace tasks

#endif  //MONITORED_TASK_H