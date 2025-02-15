#include "gpio.h"

extern "C" void SystemClock_Config(void);

int main() {
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();

    while (true) {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        HAL_Delay(1000);
    }
}