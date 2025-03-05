#include "log_task.h"
#include <cstddef>
#include "flash.h"
#include "lfs.h"
#include "littlefs.h"
#include "littlefs_file.h"
#include "portmacro.h"
#include "semihosting_stream.h"
#include "tasks/monitored_task.h"

namespace tasks {
LogTask::LogTask(flash::Flash* flash, UBaseType_t priority, StackType_t stack_size)
    : MonitoredTask("Log", stack_size, priority), file_system_(flash) {
    file_system_.Mount();
}

void LogTask::Run() {
    littlefs::File boot_count_file("boot.cnt", file_system_);
    boot_count_file.Open(LFS_O_RDWR | LFS_O_CREAT);
    size_t boot_count{0};
    auto read_result = boot_count_file.Read(&boot_count, sizeof(boot_count));
    if (read_result.has_value() && read_result.value() > 0) {
        boot_count++;
    }
    else {
        boot_count = 1;
    }
    boot_count_file.Rewind();
    boot_count_file.Write(&boot_count, sizeof(boot_count));
    boot_count_file.Close();

    mcu::semi << "Boot count: " << boot_count << "\n";

    while (true) {
        Heartbeat();
    }
}
}  // namespace tasks
