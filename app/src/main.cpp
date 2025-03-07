#include "FreeRTOS.h"
#include "gpio.h"
#include "i2c.h"
#include "iwdg.h"
#include "rtos_task.h"
#include "semihosting.h"
#include "spi.h"
#include "spi_device.h"
#include "stm32f4xx_hal.h"
#include "tasks/bme280_task.h"
#include "tasks/bmi088_task.h"
#include "tasks/log_task.h"
#include "tasks/mmc5983ma_task.h"
#include "tasks/watchdog_task.h"
#include "usb_device.h"
#include "w25n01gv.h"

extern "C" void SystemClock_Config(void);

constexpr int TASK_TIMEOUT = 800;

int main() {
    HAL_Init();
    SystemClock_Config();

    // MX_IWDG_Init();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_SPI1_Init();
    MX_USB_DEVICE_Init();

    SemihostingInit();

    static communication::SPIDevice flash_spi{&hspi1, FLASH_CS_GPIO_Port, FLASH_CS_Pin};  // NOLINT

    static flash::W25N01GV flash{&flash_spi};
    flash.Initialize();

    // static tasks::WatchdogTask watchdog_task{std::chrono::milliseconds(TASK_TIMEOUT)};
    static tasks::LogTask log_task{&flash, std::chrono::milliseconds(50), tskIDLE_PRIORITY, 1000};
    static tasks::BME280Task bme280_task;
    static tasks::BMI088Task bmi088_task;
    static tasks::MMC5983MATask mmc5983ma_task;

    bme280_task.RegisterListener(log_task);
    bmi088_task.RegisterListener(log_task);
    mmc5983ma_task.RegisterListener(log_task);
    // watchdog_task.RegisterTask(static_cast<tasks::MonitoredTask*>(&log_task));

    vTaskStartScheduler();
    while (true) {}
}