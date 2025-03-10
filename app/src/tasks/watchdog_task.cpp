#include "watchdog_task.h"
#include <cassert>
#include <chrono>
#include "iwdg.h"
#include "monitored_task.h"
#include "portmacro.h"
#include "projdefs.h"
#include "rtos_task.h"
#include "soft_assert.h"
#include "stm32f446xx.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_iwdg.h"
#include "task_configuration.h"

namespace tasks {

void WatchdogTask::Run() {

    for (auto* task : monitored_tasks_) {
        (void)task->WaitReady();
    }

    MX_IWDG_Init();
    const auto hardware_period = (hiwdg.Init.Reload + 1) / (LSI_VALUE / (1 << ((0x07 & hiwdg.Init.Prescaler) + 2))) * 1000;
    assert(hardware_period >= task_timeout_.count() && "Ensure task_timeout is less than the hardware watchdog period");

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

WatchdogTask::WatchdogTask(std::chrono::milliseconds task_timeout, StackType_t stack_size)
    : rtos::Task("Watchdog", stack_size, static_cast<UBaseType_t>(Priority::WATCHDOG)), task_timeout_(task_timeout) {
}

void WatchdogTask::RegisterTask(MonitoredTask* task) {
    if (soft_assert(task != nullptr)) {
        monitored_tasks_.push_back(task);
    }
}

}  // namespace tasks
