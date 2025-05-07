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
MAXF10STask::MAXF10STask(UART_HandleTypeDef* uart)
    : rtos::Task("MAXF10S", 2048, static_cast<UBaseType_t>(Priority::GNSS)),
      gnss_(uart) {}

void MAXF10STask::Run() {
    gnss_.Initialize(5);

    gnss::MAXF10S::Data gnss_data;
    while (true) {
        gnss_.UpdateFIFO();
        if (gnss_.Read(gnss_data)) {
            //data_provider_.NotifyListeners(gnss_data);
        }
        vTaskDelay(1);
    }
    // while(true){
    //     vTaskDelay(100000);
    // }
    
}

void MAXF10STask::RegisterListener(DataObserver<gnss::MAXF10S::Data>& observer) {
    data_provider_.Subscribe(&observer);
}

}  // namespace tasks
