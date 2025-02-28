#include "watchdog_task.h"
#include "iwdg.h"
#include "portmacro.h"
#include "projdefs.h"
#include "rtos_task.h"
#include "stm32f446xx.h"
#include "stm32f4xx_hal_iwdg.h"

namespace tasks {

void WatchdogTask::Run() {
    TickType_t last_wake_time = xTaskGetTickCount();
    const TickType_t delay_ticks = pdMS_TO_TICKS(task_timeout_.count());

    while (true) {
        for (auto* task : monitored_tasks_) {
            if (!task->IsAlive()) {
                NVIC_SystemReset();
            }
            task->ExpireHeartbeat();
        }
        HAL_IWDG_Refresh(&hiwdg);
        (void)xTaskDelayUntil(&last_wake_time, delay_ticks);
    }
}

WatchdogTask::WatchdogTask(std::chrono::milliseconds task_timeout, UBaseType_t priority, StackType_t stack_size)
    : rtos::Task("Watchdog", stack_size, priority), task_timeout_(task_timeout) {}

void WatchdogTask::RegisterTask(MonitoredTask* task) {
    monitored_tasks_.push_back(task);
}

}  // namespace tasks
