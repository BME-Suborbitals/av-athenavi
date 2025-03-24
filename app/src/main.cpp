#include "FreeRTOS.h"
#include "communication/i2c_threadsafe.h"
#include "communication/spi_threadsafe_dma.h"
#include "dma.h"
#include "gpio.h"
#include "i2c.h"
#include "iwdg.h"
#include "littlefs.h"
#include "semihosting.h"
#include "spi.h"
#include "spi_device.h"
#include "stm32f4xx_hal.h"
// #include "tasks/bme280_task.h"
#include "littlefs_file.h"
#include "tasks/bmi088_task.h"
#include "tasks/cli_task.h"
#include "tasks/log_task.h"
#include "tasks/mmc5983ma_task.h"
#include "tasks/ms5611_task.h"
#include "tasks/task_configuration.h"
#include "tasks/watchdog_task.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "w25n01gv.h"

extern "C" void SystemClock_Config(void);

int main() {
    HAL_Init();
    SystemClock_Config();

    MX_DMA_Init();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_SPI1_Init();
    MX_USB_DEVICE_Init();

    SemihostingInit();

    static communication::SPIThreadsafeDMA flash_spi{&hspi1, FLASH_CS_GPIO_Port, FLASH_CS_Pin};  // NOLINT
    static flash::W25N01GV flash{&flash_spi};
    flash.Initialize();

    static littlefs::LittleFS file_system{&flash};
    file_system.Mount();

    static tasks::WatchdogTask watchdog_task{tasks::TASK_TIMEOUT};
    static tasks::LogTask log_task{file_system, tasks::LOG_FREQUENCY, 1000};
    // static tasks::BME280Task bme280_task;
    static tasks::MS5611Task ms5611_task;
    static tasks::BMI088Task bmi088_task;
    static tasks::MMC5983MATask mmc5983ma_task;
    static tasks::CLITask cli_task{file_system};

    // bme280_task.RegisterListener(log_task);
    ms5611_task.RegisterListener(log_task);
    bmi088_task.RegisterListener(log_task);
    mmc5983ma_task.RegisterListener(log_task);
    watchdog_task.RegisterTask(static_cast<tasks::MonitoredTask*>(&log_task));

    vTaskStartScheduler();
}