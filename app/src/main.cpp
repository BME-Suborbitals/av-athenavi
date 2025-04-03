#include "communication/spi_threadsafe_dma.h"
#include "dma.h"
#include "gpio.h"
#include "i2c.h"
#include "littlefs.h"
#include "semihosting.h"
#include "spi.h"
#include "stm32f4xx_hal.h"
#include "tasks/task_manager.h"
#include "usart.h"
#include "usb_device.h"
#include "w25n01gv.h"

#include "maxf10s.h"
#include "semihosting_stream.h"

extern "C" void SystemClock_Config(void);

int main() {
    HAL_Init();
    SystemClock_Config();

    MX_DMA_Init();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_USART3_UART_Init();
    MX_USB_DEVICE_Init();

    SemihostingInit();

    // char buff[256];
    // HAL_GPIO_WritePin(GNSS_RST_GPIO_Port, GNSS_RST_Pin, GPIO_PIN_RESET);
    // HAL_Delay(10);
    // HAL_GPIO_WritePin(GNSS_RST_GPIO_Port, GNSS_RST_Pin, GPIO_PIN_SET);

    // while (true) {
    //     volatile auto res = HAL_UART_Receive(&huart1, (uint8_t*)buff, 256, 10000);
    // }

    gnss::MAXF10S gnss;
    volatile auto init = gnss.Initialize();

    std::vector<gnss::MAXF10S::Data> data;
    while (true) {
        gnss.Read(data);
        for (auto point : data) {
            mcu::semi << point.time << ", " << point.lat << ", " << point.lon << "\n";
        }
    }

    // static communication::SPIThreadsafeDMA flash_spi{&hspi1, FLASH_CS_GPIO_Port, FLASH_CS_Pin};  // NOLINT
    // static flash::W25N01GV flash{&flash_spi};
    // flash.Initialize();

    // static littlefs::LittleFS file_system{&flash};
    // file_system.Mount();

    // tasks::TaskManager::GetInstance().Start(file_system);
}
