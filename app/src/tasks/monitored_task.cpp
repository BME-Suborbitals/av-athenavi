#include "monitored_task.h"
#include <cstdint>
#include "portmacro.h"
#include "projdefs.h"
#include "rtos_task.h"

namespace tasks {
void tasks::MonitoredTask::Heartbeat() {
    is_alive_ = true;
}

void MonitoredTask::NotReady() {
    xSemaphoreTake(is_ready_, 0);
}

void MonitoredTask::Ready() {
    xSemaphoreGive(is_ready_);
}

MonitoredTask::MonitoredTask(const char* name, StackType_t stack_size, UBaseType_t priority)
    : rtos::Task(name, stack_size, priority),
      is_ready_(xSemaphoreCreateBinary()) {}

bool MonitoredTask::IsAlive() const {
    return is_alive_;
}

void MonitoredTask::ExpireHeartbeat() {
    is_alive_ = false;
}

bool MonitoredTask::WaitReady(uint32_t timeout) {
    if (xSemaphoreTake(is_ready_, timeout) == pdTRUE) {
        xSemaphoreGive(is_ready_);
        return true;
    }
    return false;
}
}  // namespace tasks
