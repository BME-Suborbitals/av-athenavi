#include "maxf10s_task.h"
#include <cstdint>
#include "FreeRTOSConfig.h"
#include "portmacro.h"
#include "projdefs.h"
#include "rtos_task.h"
#include "task_configuration.h"
#include "tasks/data_observer.h"

namespace tasks {
MAXF10STask::MAXF10STask()
    : rtos::Task("MAXF10S", 2000, static_cast<UBaseType_t>(Priority::SENSOR)) { }

void MAXF10STask::Run() {
    gnss_.Initialize();

    // std::vector<gnss::MAXF10S::Data> gnss_data{};
    std::vector<gnss::MAXF10S::Data> gnss_data;
    while (true) {

        if (gnss_.Read(gnss_data)) {
            for(int i = 0; i < gnss_data.size(); i++){
                data_provider_.NotifyListeners(gnss_data[i]);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void MAXF10STask::RegisterListener(DataObserver<gnss::MAXF10S::Data>& observer) {
    data_provider_.Subscribe(&observer);
}

}  // namespace tasks
