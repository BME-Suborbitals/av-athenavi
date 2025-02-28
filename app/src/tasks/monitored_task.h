#ifndef MONITORED_TASK_H
#define MONITORED_TASK_H

#include "FreeRTOS.h"
#include "portmacro.h"
#include "rtos_task.h"

namespace tasks {
class MonitoredTask : public rtos::Task {
  private:
    bool is_alive_{true};

  protected:
    void Heartbeat();

  public:
    MonitoredTask(const char* name, StackType_t stack_size, UBaseType_t priority);
    [[nodiscard]] bool IsAlive() const;
    void ExpireHeartbeat();
};
}  // namespace tasks

#endif  //MONITORED_TASK_H