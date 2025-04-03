#include "maxf10s_task.h"
#include <cstdint>
#include "FreeRTOSConfig.h"
#include "portmacro.h"
#include "projdefs.h"
#include "rtos_task.h"
#include "task_configuration.h"
#include "tasks/data_observer.h"
#include "uart_device.h"
#include "semihosting.h"
#include "semihosting_stream.h"

namespace tasks {
MAXF10STask::MAXF10STask(communication::UARTDevice* uart)
    : rtos::Task("MAXF10S", 2000, static_cast<UBaseType_t>(Priority::SENSOR)),
    gnss_(uart){ }

void MAXF10STask::Run() {
    gnss_.Initialize();

    gnss::MAXF10S::Data gnss_data;

    while (true) {

        if (gnss_.Read(gnss_data)) {
            data_provider_.NotifyListeners(gnss_data);
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void MAXF10STask::RegisterListener(DataObserver<gnss::MAXF10S::Data>& observer) {
    data_provider_.Subscribe(&observer);
}

}  // namespace tasks
