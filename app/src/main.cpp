#include "gpio.h"
#include "bmi088.h"
#include "bme280.h"
#include "mmc5983ma.h"
#include "i2c.h"
#include "i2c_device.h"
#include "serial.h"

extern "C" void SystemClock_Config(void);

#define SENSOR_BMI088
#define SENSOR_BME280
#define SENSOR_MMC5986

int main() {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_USART2_UART_Init();

    #ifdef SENSOR_MMC5986
    communication::I2CDevice i2c_mmc(hi2c1, 0x30);
    sensor::MMC5983MA mmc(&i2c_mmc);
    if(!mmc.Initialize(sensor::MMC5983MA::BANDWIDTH_100_HZ)){
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    }
    #endif

    #ifdef SENSOR_BME280
    communication::I2CDevice i2c_bme(hi2c1, 0x76);
    sensor::BME280 bme(&i2c_bme);
    if(!bme.Initialize(sensor::BME280::Oversampling::SAMPLE_STANDARD,
                       sensor::BME280::Oversampling::SAMPLE_STANDARD,
                       sensor::BME280::Oversampling::SAMPLE_STANDARD,
                       sensor::BME280::PowerMode::MODE_NORMAL,
                       sensor::BME280::StandbyTime::STANDBY_62_5_MS,
                       sensor::BME280::Filter::FILTER_2)){
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    }
    #endif

    #ifdef SENSOR_BMI088
    communication::I2CDevice i2c_acc(hi2c1, 0x18);
    communication::I2CDevice i2c_gyro(hi2c1, 0x68);
    sensor::BMI088 bmi088(&i2c_acc, &i2c_gyro);
    if(!bmi088.Initialize(sensor::BMI088::ACC_RANGE_12_G,
                         sensor::BMI088::SAMPLE_NORMAL,
                         sensor::BMI088::ACC_ODR_100_HZ,
                         sensor::BMI088::GYRO_RANGE_2000_DPS,
                         sensor::BMI088::GYRO_ODR_1000_FBW_116)){
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    }
    #endif
    
    while (true) {
        #ifdef SENSOR_BME280
        sensor::BME280::Data bme_data;
        bme.Read(bme_data);
        uart_debug::print("Pres = ");
        uart_debug::print(bme_data.pressure);
        uart_debug::print("\tHum = ");
        uart_debug::print(bme_data.humidity);
        uart_debug::print("\tTemp = ");
        uart_debug::println(bme_data.temperature);
        #endif

        #ifdef SENSOR_MMC5986
        sensor::MMC5983MA::Data mmc_data;
        mmc.Read(mmc_data);
        uart_debug::print("mX = ");
        uart_debug::print(mmc_data.magnetic_field_x);
        uart_debug::print("\tmY = ");
        uart_debug::print(mmc_data.magnetic_field_y);
        uart_debug::print("\tmZ = ");
        uart_debug::println(mmc_data.magnetic_field_z);
        #endif

        #ifdef SENSOR_BMI088
        sensor::BMI088::Data bmi_data;
        bmi088.Read(bmi_data);
        uart_debug::print("Time = ");
        uart_debug::print(bmi_data.time);
        uart_debug::print("\tTemp = ");
        uart_debug::print(bmi_data.temperature);
        uart_debug::print("\taX = ");
        uart_debug::print(bmi_data.acceleration_x);
        uart_debug::print("\taY = ");
        uart_debug::print(bmi_data.acceleration_y);
        uart_debug::print("\taZ = ");
        uart_debug::print(bmi_data.acceleration_z);
        uart_debug::print("\tgX = ");
        uart_debug::print(bmi_data.angular_velocity_x);
        uart_debug::print("\tgY = ");
        uart_debug::print(bmi_data.angular_velocity_y);
        uart_debug::print("\tgZ = ");
        uart_debug::println(bmi_data.angular_velocity_x);
        #endif

        uart_debug::println();
        HAL_Delay(200);
    }
}