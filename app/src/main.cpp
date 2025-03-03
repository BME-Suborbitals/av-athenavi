#include "gpio.h"
#include "bme280.h"
// #include "mmc5983ma.h"
#include "i2c.h"
#include "i2c_device.h"
#include "serial.h"

extern "C" void SystemClock_Config(void);

int main() {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_USART2_UART_Init();

    // communication::I2CDevice i2c_mmc(hi2c1, 0x30);
    // sensor::MMC5983MA mmc(&i2c_mmc);
    // mmc.Initialize(sensor::MMC5983MA::BANDWIDTH_100_HZ);

    communication::I2CDevice i2c_bme(hi2c1, 0x77);
    sensor::BME280 bme(&i2c_bme);
    if(!bme.Initialize(sensor::BME280::Oversampling::SAMPLE_STANDARD,
                       sensor::BME280::Oversampling::SAMPLE_STANDARD,
                       sensor::BME280::Oversampling::SAMPLE_STANDARD,
                       sensor::BME280::PowerMode::MODE_NORMAL,
                       sensor::BME280::StandbyTime::STANDBY_62_5_MS,
                       sensor::BME280::Filter::FILTER_2)){
        while(1);
    }
    

    while (true) {
        // sensor::MMC5983MA::Data mmc_data;
        // mmc.Read(mmc_data);

        sensor::BME280::Data bmp_data;
        bme.Read(bmp_data);
        uart_debug::print("Pres = ");
        uart_debug::print(bmp_data.pressure);
        uart_debug::print("\tTemp = ");
        uart_debug::println(bmp_data.temperature);

        // uart_debug::print("mX = ");
        // uart_debug::print(mmc_data.magnetic_field_x);
        // uart_debug::print("\tmY = ");
        // uart_debug::print(mmc_data.magnetic_field_y);
        // uart_debug::print("\tmZ = ");
        // uart_debug::println(mmc_data.magnetic_field_z);

        //HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        HAL_Delay(125);
    }
}