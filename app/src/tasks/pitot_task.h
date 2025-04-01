#ifndef PITOT_TASK_H
#define PITOT_TASK_H

#include "data_observer.h"
#include "data_provider.h"
#include "rtos_task.h"

namespace tasks {
class PitotTask : public rtos::Task {
  public:
    struct PitotData {
        uint16_t channel1;
        uint16_t channel2;
    };

    PitotTask();
    void Run() override;
    void RegisterListener(DataObserver<PitotData>& observer);

  private:
    DataProvider<PitotData> data_provider_;
};
}  // namespace tasks

#endif  //PITOT_TASK_H