#include "FreeRTOS.h"
#include "gpio.h"
#include "i2c.h"
#include "spi.h"
#include "stm32f4xx_hal.h"
#include "usb_device.h"

extern "C" void SystemClock_Config(void);

int main() {
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_SPI1_Init();
    MX_USB_DEVICE_Init();

    while (true) {}
}