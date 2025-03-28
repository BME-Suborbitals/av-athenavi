#include "task_manager.h"
#include "FreeRTOS.h"
#include "communication/i2c_threadsafe.h"
#include "littlefs.h"
#include "task_configuration.h"
#include "tasks/bmi088_task.h"
#include "tasks/cli_task.h"
#include "tasks/log_task.h"
#include "tasks/mmc5983ma_task.h"
#include "tasks/ms5611_task.h"
#include "tasks/watchdog_task.h"

namespace tasks {
TaskManager TaskManager::instance;

void TaskManager::Start(littlefs::LittleFS& file_system) {
    static tasks::BMI088Task bmi088_task;
    static tasks::MMC5983MATask mmc5983ma_task;
    static tasks::MS5611Task ms5611_task;

    static tasks::LogTask log_task{file_system, LOG_FREQUENCY, LOG_TASK_STACK};
    static tasks::WatchdogTask watchdog_task{TASK_TIMEOUT};

    static tasks::CLITask cli_task{file_system};

    bmi088_task_ = &bmi088_task;
    mmc5983ma_task_ = &mmc5983ma_task;
    ms5611_task_ = &ms5611_task;
    log_task_ = &log_task;
    watchdog_task_ = &watchdog_task;
    cli_task_ = &cli_task;

    watchdog_task.RegisterTask(&log_task);
    bmi088_task.RegisterListener(log_task);
    mmc5983ma_task.RegisterListener(log_task);
    ms5611_task.RegisterListener(log_task);

    vTaskStartScheduler();
}

TaskManager& TaskManager::GetInstance() {
    return instance;
}
}  // namespace tasks
