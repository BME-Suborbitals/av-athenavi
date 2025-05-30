#include "maxf10s_task.h"
#include <cstdint>
#include "FreeRTOSConfig.h"
#include "communication/uart_device_dma.h"
#include "portmacro.h"
#include "projdefs.h"
#include "rtos_task.h"
#include "semihosting.h"
#include "semihosting_stream.h"
#include "task_configuration.h"
#include "tasks/data_observer.h"

namespace tasks {
MAXF10STask::MAXF10STask()
    : rtos::Task("MAXF10S", 2000, static_cast<UBaseType_t>(Priority::SENSOR)), gnss_(&huart3) {}

void MAXF10STask::Run() {
    TickType_t last_wake_time = xTaskGetTickCount();
    const TickType_t delay_ticks = pdMS_TO_TICKS(100);

    gnss_.Initialize(5);
    gnss::MAXF10S::Data gnss_data;
    
    while (true) {
        gnss_.UpdateFIFO();
        while (gnss_.Read(gnss_data)) {
            // mcu::semi << "-\n";
            data_provider_.NotifyListeners(gnss_data);
        }
        xTaskDelayUntil(&last_wake_time, delay_ticks);
    }
}

void MAXF10STask::RegisterListener(DataObserver<gnss::MAXF10S::Data>& observer) {
    data_provider_.Subscribe(&observer);
}

}  // namespace tasks
