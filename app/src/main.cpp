#include "FreeRTOS.h"
#include "communication/i2c_threadsafe.h"
#include "gpio.h"
#include "i2c.h"
#include "iwdg.h"
#include "littlefs.h"
#include "semihosting.h"
#include "semihosting_stream.h"
#include "spi.h"
#include "spi_device.h"
#include "stm32f4xx_hal.h"
// #include "tasks/bme280_task.h"
#include "littlefs_file.h"
// #include "tasks/bmi088_task.h"
// #include "tasks/log_task.h"
// #include "tasks/mmc5983ma_task.h"
// #include "tasks/ms5611_task.h"
// #include "tasks/task_configuration.h"
// #include "tasks/usb_task.h"
#include "tasks/maxf10s_task.h"
#include "tasks/watchdog_task.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "w25n01gv.h"
#include "uart_device.h"

extern "C" void SystemClock_Config(void);

int main() {
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    // MX_I2C1_Init();
    // MX_SPI1_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    // MX_USB_DEVICE_Init();

    // SemihostingInit();

    mcu::semi << "\nNew run\n";
    // usb_debug::println("New run");
    // static communication::SPIDevice flash_spi{&hspi1,        , FLASH_CS_Pin};  // NOLINT
    // static flash::W25N01GV flash{&flash_spi};
    // flash.Initialize();

    // static littlefs::LittleFS file_system{&flash};
#ifdef FORMAT_FLASH
    file_system.Format();
#endif
    // file_system.Mount();

#ifdef DUMP_LOGS
    HAL_Delay(4000);
    file_system.ForEachFile([](const littlefs::FileInfo& file) {
        if (strncmp(file.name, "log-", 4) == 0) {
            littlefs::File log_file{file.name, file_system};
            log_file.Open(LFS_O_RDONLY);
            tasks::LogEntry log_entry;
            char buff[150];
            size_t num_records = file.size / sizeof(log_entry);
            for (size_t i = 0; i < num_records; i++) {
                log_file.Read(&log_entry, sizeof(log_entry));
                sprintf(buff, "%s;%d;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f\n", file.name, log_entry.timestamp, log_entry.imu_data.time, log_entry.imu_data.temperature, log_entry.imu_data.acceleration_x, log_entry.imu_data.acceleration_y, log_entry.imu_data.acceleration_z, log_entry.imu_data.angular_velocity_x, log_entry.imu_data.angular_velocity_y, log_entry.imu_data.angular_velocity_z, log_entry.barometric_data.pressure, log_entry.barometric_data.temperature, log_entry.magnetometer_data.magnetic_field_x, log_entry.magnetometer_data.magnetic_field_y, log_entry.magnetometer_data.magnetic_field_z);
                CDC_Transmit_FS((uint8_t*)buff, strlen(buff));
                HAL_Delay(1);
            }
            log_file.Close();
        }
    });

#else
    // static tasks::WatchdogTask watchdog_task{tasks::TASK_TIMEOUT};
    // static tasks::LogTask log_task{file_system, tasks::LOG_FREQUENCY, 1000};
    // static tasks::BME280Task bme280_task;
    // static tasks::MS5611Task ms5611_task;
    // static tasks::BMI088Task bmi088_task;
    // static tasks::MMC5983MATask mmc5983ma_task;
    static tasks::MAXF10STask maxf10s_task;

    // bme280_task.RegisterListener(log_task);
    // ms5611_task.RegisterListener(log_task);
    // bmi088_task.RegisterListener(log_task);
    // mmc5983ma_task.RegisterListener(log_task);
    // watchdog_task.RegisterTask(static_cast<tasks::MonitoredTask*>(&log_task));

    vTaskStartScheduler();
#endif
    while (true) {}
}

