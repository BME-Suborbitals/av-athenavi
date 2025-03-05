#ifndef LOG_TASK_H
#define LOG_TASK_H

#include "flash.h"
#include "littlefs.h"
#include "monitored_task.h"

namespace tasks {
class LogTask : public tasks::MonitoredTask {
  private:
    littlefs::LittleFS file_system_;

  public:
    LogTask(flash::Flash* flash, UBaseType_t priority, StackType_t stack_size);
    void Run() override;
};
}  // namespace tasks

#endif  //LOG_TASK_H