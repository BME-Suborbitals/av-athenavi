#include "monitored_task.h"
#include "portmacro.h"
#include "rtos_task.h"

namespace tasks {
void tasks::MonitoredTask::Heartbeat() {
    is_alive_ = true;
}

MonitoredTask::MonitoredTask(const char* name, StackType_t stack_size, UBaseType_t priority)
    : rtos::Task(name, stack_size, priority) {}

bool MonitoredTask::IsAlive() const {
    return is_alive_;
}

void MonitoredTask::ExpireHeartbeat() {
    is_alive_ = false;
}
}  // namespace tasks
