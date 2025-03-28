#include "communication/spi_threadsafe_dma.h"
#include "dma.h"
#include "gpio.h"
#include "i2c.h"
#include "littlefs.h"
#include "semihosting.h"
#include "spi.h"
#include "stm32f4xx_hal.h"
#include "tasks/task_manager.h"
#include "usb_device.h"
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

    tasks::TaskManager::GetInstance().Start(file_system);
}